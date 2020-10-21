/**
 *  This file is part of Bazcal.
 *
 *  Bazcal is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Bazcal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Bazcal.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bazcal.h"
#include "fetch.h"
#include "parser.h"
#include "db.h"
#include "calc.h"
#include "ahpredict.h"
#include <stdio.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include <string.h>

int main () {
    int page = 0;

    sqlite3 *db;
    int rc = sqlite3_open("test.db", &db);
    bz_init_db(db);

    bz_bazaar_t *data = bz_bazaar_init();

    bz_fetch_res_t *baz_res = fetch_init_res();
    fetch("https://api.hypixel.net/skyblock/bazaar", baz_res);
    bz_parse_bazaar(baz_res, data);

    for (size_t x = 0; x < data->count; x++) {
        printf("%s\n", data->items[x]->item_name);
    }

    bazaar_free(data);
    fetch_free(baz_res);

    while (1) {
        // Generate the predictions
        if (page == 0) {
            size_t predictions_len = 0;
            printf("[libbazcal] Generating auction predictions...\n");
            bz_prediction_t **predictions = bz_generate_predictions(db, &predictions_len);
            printf("[libbazcal] Generated %zu predictions for items\n", predictions_len);
            for (size_t i = 0; i < predictions_len; i++) {
                printf("[libbazcal] Prediction: %s,%d,%.2f\n", predictions[i]->item_name, predictions[i]->n, predictions[i]->value);
            }
            bz_auction_pool_t **pool = bz_populate_auction_pool(db, predictions, predictions_len);
            bz_auction_pool_t *random_flips = bz_random_auction_flips(pool, predictions_len, 0, 0, RAND_MAX, 50, 6, NULL);
            printf("[libbazcal] %zu random predictions\n", random_flips->size);
            bz_free_random_auction_flips(random_flips);
            bz_free_predictions(predictions, predictions_len);
        }

        // Fetch the api data
        bz_fetch_res_t *res = fetch_init_res();
        int url_len = snprintf(NULL, 0, "https://api.hypixel.net/skyblock/auctions?page=%d", page);
        char *url = malloc(url_len);
        sprintf(url, "https://api.hypixel.net/skyblock/auctions?page=%d", page);
        printf("[libbazcal] Processing url \"%s\"\n", url);
        fetch(url, res);
        free(url);

        // Parse the api data
        bz_auction_page_t *parsed = bz_auction_page_init();
        bz_parse_auctions(res, parsed);

        // Save to db
        bz_add_auc_page(db, parsed);

        if (parsed->page == parsed->pages) {
            page = 0;
        } else {
            page++;
        }

        // Cleanup
        auction_page_free(parsed);
        fetch_free(res);
    }
}