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