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

#ifndef BZ_REGRESSION_H
#define BZ_REGRESSION_H

#include "bazcal.h" 

#ifdef __cplusplus
extern "C" {
#endif

double quick_select (double *arr, size_t n);

struct bz_regression_s {
    double slope;
    double intercept;
};

typedef struct bz_regression_s bz_regression_t;

void bz_theil_sen (bz_regression_t *regression, bid_t *x, time_t *y, size_t len);
double bz_predict (bz_regression_t *model, bid_t x);

#ifdef __cplusplus
}
#endif

#endif