#ifndef BZ_REGRESSION_H
#define BZ_REGRESSION_H

#include "bazcal.h" 

double quick_select (double *arr, size_t n);

struct bz_regression_s {
    double slope;
    double intercept;
};

typedef struct bz_regression_s bz_regression_t;

void bz_theil_sen (bz_regression_t *regression, bid_t *x, time_t *y, size_t len);
double bz_predict (bz_regression_t *model, bid_t x);

#endif