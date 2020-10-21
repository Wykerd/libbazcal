#ifndef BZ_CALC_H
#define BZ_CALC_H

#include "bazcal.h"
#include "regression.h"
#include <time.h>
#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bz_prediction_s {
    char* item_name;
    size_t n;
    double value;
};

typedef struct bz_prediction_s bz_prediction_t;

bz_prediction_t **bz_generate_predictions (sqlite3 *db, size_t *len);

void bz_free_predictions (bz_prediction_t **predictions, size_t len);

#ifdef __cplusplus
}
#endif

#endif