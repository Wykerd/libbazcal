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

#ifndef BZ_LOOP_H
#define BZ_LOOP_H

#include "bazcal.h"

#ifdef __cplusplus
extern "C" {
#endif

enum bz_log_level {
    LOG_FETCH = 1,
    LOG_GENERATE_PREDICTION = 2,
    LOG_PREDICTION_COUNT = 4,
    LOG_ITEM_PREDICTIONS = 8
};

typedef enum bz_log_level bz_log_level_t;

void bz_auction_loop (const char* database_name, bz_log_level_t log_level, void(*prediction_callback)(bz_auction_pool_t **, size_t));

#ifdef __cplusplus
}
#endif

#endif