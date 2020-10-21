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

#include "regression.h"
#include <stdlib.h>


void bz_theil_sen (bz_regression_t *regression, bid_t *x, time_t *y, size_t len) {
    double *slopes = malloc(len * len * sizeof(bid_t));

    size_t index = 0;
    for (size_t i = 0; i < len; i++) {
        for (size_t j = i + 1; j < len; j++) {
            slopes[index++] = (y[j] - y[i]) / ((x[j] - x[i]) * 1.0); // force floating point math
        }
    }

    double median_slope = quick_select(slopes, len * len);

    free(slopes);

    double *cuts = malloc(len * sizeof(double));

    for (size_t i = 0; i < len; ++i) {
        cuts[i] = y[i] - median_slope * x[i];
    };

    regression->slope = median_slope;
    regression->intercept = quick_select(cuts, len);

    free(cuts);
};

double bz_predict (bz_regression_t *model, bid_t x) {
    return model->slope * x + model->intercept;
}