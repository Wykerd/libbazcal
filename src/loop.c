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

#include "loop.h"
#include "fetch.h"
#include "parser.h"
#include "db.h"
#include "calc.h"
#include "ahpredict.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

void bz_auction_loop (const char* database_name, bz_log_level_t log_level, void(*prediction_callback)(bz_auction_pool_t **, size_t)) {
    int page = 0;

    sqlite3 *db;
    int rc = sqlite3_open(database_name, &db);
    bz_init_db(db);

    while (1) {
        // Generate the predictions
        if (page == 0) {
            size_t predictions_len = 0;
            if (log_level & LOG_GENERATE_PREDICTION) printf("[libbazcal] Generating auction predictions...\n");
            bz_prediction_t **predictions = bz_generate_predictions(db, &predictions_len);
            if (log_level & LOG_PREDICTION_COUNT) printf("[libbazcal] Generated %zu predictions for items\n", predictions_len);
            if (log_level & LOG_ITEM_PREDICTIONS)
                for (size_t i = 0; i < predictions_len; i++) {
                    printf("[libbazcal] Prediction: %s,%d,%.2f\n", predictions[i]->item_name, predictions[i]->n, predictions[i]->value);
                }
            size_t pool_len = 0;
            bz_auction_pool_t **pool = bz_populate_auction_pool(db, predictions, predictions_len, &pool_len);
            (*prediction_callback)(pool, pool_len);
            bz_free_predictions(predictions, predictions_len);
        }

        // Fetch the api data
        bz_fetch_res_t *res = fetch_init_res();
        int url_len = snprintf(NULL, 0, "https://api.hypixel.net/skyblock/auctions?page=%d", page);
        char *url = malloc(url_len);
        sprintf(url, "https://api.hypixel.net/skyblock/auctions?page=%d", page);
        if (log_level & LOG_FETCH) printf("[libbazcal] Processing url \"%s\"\n", url);
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