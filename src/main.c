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
#include "calc.h"
#include "loop.h"
#include <stdio.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include <string.h>

void bz_auction_callback (bz_auction_pool_t **pool, size_t size) {
    bz_auction_pool_t *random_flips = bz_random_auction_flips(pool, size, 0, 0, RAND_MAX, 50, 6, NULL);
    printf("[libbazcal] %zu random predictions\n", random_flips->size);
    bz_free_random_auction_flips(random_flips);
    bz_free_auction_pool(pool, size);
}

int main () {
    bz_bazaar_t *data = bz_bazaar_init();

    bz_fetch_res_t *baz_res = fetch_init_res();
    fetch("https://api.hypixel.net/skyblock/bazaar", baz_res);
    bz_parse_bazaar(baz_res, data);

    for (size_t x = 0; x < data->count; x++) {
        printf("%s\n", data->items[x]->item_name);
    }

    bazaar_free(data);
    fetch_free(baz_res);

    bz_auction_loop("test.db", LOG_FETCH | LOG_GENERATE_PREDICTION | LOG_PREDICTION_COUNT | LOG_ITEM_PREDICTIONS, &bz_auction_callback);
}