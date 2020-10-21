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

#ifndef BZ_CALC_H
#define BZ_CALC_H

#include "bazcal.h"
#include "regression.h"
#include <time.h>
#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

bz_prediction_t **bz_generate_predictions (sqlite3 *db, size_t *len);

void bz_free_predictions (bz_prediction_t **predictions, size_t len);

#ifdef __cplusplus
}
#endif

#endif