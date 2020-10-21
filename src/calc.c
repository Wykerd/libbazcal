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

#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static time_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (time_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
};

bz_prediction_t **bz_generate_predictions (sqlite3 *db, size_t *len) {
    sqlite3_stmt *stmt;
    int rc;
    time_t current_time = get_time_ms();

    rc = sqlite3_prepare(db, 
        "SELECT "
            "item_full_name,"
            "COUNT(max_bid) "
        "FROM auctions "
        "WHERE end < ? AND has_bid > 0 "
        "GROUP BY item_full_name "
        "HAVING COUNT(max_bid) > 49"
    , -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "DB prediction prepare error: %s\n", sqlite3_errmsg (db));

        return NULL;
    };

    CHECK_BIND_OK_NULL(sqlite3_bind_int64(stmt, 1, current_time));

    bz_prediction_t **predictions = malloc(sizeof(bz_prediction_t *));
    size_t predictions_len = 0;

    rc = sqlite3_step(stmt);

    while (rc == SQLITE_ROW) {
        const char* item_name = sqlite3_column_text(stmt, 0);
        
        sqlite3_stmt *_stmt;

        rc = sqlite3_prepare(db, 
            "SELECT "
                "end,"
                "max_bid "
            "FROM auctions "
            "WHERE end < ? AND has_bid > 0 AND item_full_name = ? "
            "LIMIT 10000"
        , -1, &_stmt, 0);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "DB prediction prepare item error: %s\n", sqlite3_errmsg (db));

            return NULL;
        };

        CHECK_BIND_OK_NULL(sqlite3_bind_int64(_stmt, 1, current_time));
        CHECK_BIND_OK_NULL(sqlite3_bind_text(_stmt, 2, item_name, -1, NULL));

        bid_t *bids = malloc(sizeof(bid_t));
        time_t *ends = malloc(sizeof(time_t));
        size_t length = 0;

        rc = sqlite3_step(_stmt);

        while (rc == SQLITE_ROW) {
            bids = realloc(bids, ++length * sizeof(bid_t*));
            ends = realloc(ends, length * sizeof(time_t*));
            
            bids[length - 1] = sqlite3_column_int64(_stmt, 0);
            ends[length - 1] = sqlite3_column_int64(_stmt, 1);

            rc = sqlite3_step(_stmt);
        }

        bz_regression_t regression;

        bz_theil_sen(&regression, bids, ends, length);

        predictions = realloc(predictions, ++predictions_len * sizeof(bz_prediction_t *));

        predictions[predictions_len - 1] = malloc(sizeof(bz_prediction_t));

        predictions[predictions_len - 1]->item_name = calloc(sizeof(char), strlen(item_name) + 1);

        strcpy(predictions[predictions_len - 1]->item_name, item_name);

        predictions[predictions_len - 1]->n = length;
        predictions[predictions_len - 1]->value = bz_predict(&regression, current_time);

        // cleanup
        free(bids);
        free(ends);
        sqlite3_finalize(_stmt);

        // next item
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    *len = predictions_len;

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "WARNING: sql return status is not SQLITE_DONE at calc.c:101\n");
    }

    return predictions;
}

void bz_free_predictions (bz_prediction_t **predictions, size_t len) {
    for (size_t i = 0; i < len; i++) {
        free(predictions[i]->item_name);
        free(predictions[i]);
    };
    free(predictions);
}