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

#ifndef BZ_AHPREDICT_H
#define BZ_AHPREDICT_H

#include "bazcal.h"
#include "calc.h"
#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

bz_auction_pool_t **bz_populate_auction_pool (sqlite3 *db, bz_prediction_t **predictions, size_t len);

bz_auction_pool_t *bz_random_auction_flips (
    bz_auction_pool_t **pool, 
    size_t len, 
    bid_t max_bid, 
    double min_profit, 
    time_t max_time_left, 
    int min_dataset, 
    size_t try_item_amount, 
    char **ignore_item
);

void bz_free_auction_pool (bz_auction_pool_t **pool, size_t len);
void bz_free_random_auction_flips (bz_auction_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif