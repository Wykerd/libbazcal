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
#include "bazcal/fetch.h"
#include "bazcal/parser.h"
#include <stdio.h>
#include <sqlite3.h>

void loop_callback (sqlite3 *db) {
    size_t predictions_len = 0;
    printf("[libbazcal] Generating auction predictions...\n");
    bz_prediction_t **predictions = bz_generate_predictions(db, &predictions_len);
    printf("[libbazcal] Generated %zu predictions for items\n", predictions_len);
    for (size_t i = 0; i < predictions_len; i++) {
        printf("[libbazcal] Prediction: %s,%d,%.2f\n", predictions[i]->item_name, predictions[i]->n, predictions[i]->value);
    }
    size_t pool_len = 0;
    bz_auction_pool_t **pool = bz_populate_auction_pool(db, predictions, predictions_len, &pool_len);
    bz_auction_pool_t *random_flips = bz_random_auction_flips(pool, pool_len, 0, 0, RAND_MAX, 0, 50, 6, NULL);
    printf("[libbazcal] %zu random predictions\n", random_flips->size);
    bz_free_random_auction_flips(random_flips);
    bz_free_auction_pool(pool, pool_len);
    bz_free_predictions(predictions, predictions_len);
}

int main () {
    bz_bazaar_t *data = bz_bazaar_init();

    bz_fetch_res_t *baz_res = fetch_init_res();
    fetch("https://api.hypixel.net/skyblock/bazaar", baz_res);
    bz_parse_bazaar(baz_res, data);
    fetch_free(baz_res);

    for (size_t x = 0; x < data->count; x++) {
        printf("%s\n", data->items[x]->item_name);
    }

    bz_bazaar_advice_buf_t *items = bz_advise(data, 10000, 5);

    for (size_t x = 0; x < items->count; x++) {
        printf(">>> %s\n", items->items[x]->name);
    }

    bz_free_advise(items);
    bz_free_bazaar(data);

    bz_auction_loop("test.db", 1, &loop_callback);
}