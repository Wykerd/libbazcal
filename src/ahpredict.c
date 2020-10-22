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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAST_CANDIDATE pool[pool_len-1]->candidates[pool[pool_len-1]->size-1]
#define random_range(lower, upper) ((rand() % (upper - lower + 1)) + lower)

static time_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (time_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
};

bz_auction_pool_t **bz_populate_auction_pool (sqlite3 *db, bz_prediction_t **predictions, size_t len, size_t *poollen) {
    bz_auction_pool_t **pool = malloc(sizeof(bz_auction_pool_t *));
    size_t pool_len = 0;

    time_t current_time = get_time_ms();

    for (size_t i = 0; i < len; i++) {
        sqlite3_stmt *stmt;
        int rc;

        rc = sqlite3_prepare(db, 
            "SELECT "
                "uuid,auctioneer,start,end,max_bid,has_bid,item_full_name,item_name,bin "
            "FROM auctions "
            "WHERE end > ? AND item_full_name = ?"
        , -1, &stmt, 0);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "DB pool populate prepare error: %s\n", sqlite3_errmsg (db));

            return NULL;
        };

        CHECK_BIND_OK_NULL(sqlite3_bind_int64(stmt, 1, current_time));
        CHECK_BIND_OK_NULL(sqlite3_bind_text(stmt, 2, predictions[i]->item_name, -1, NULL));

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW) {
            pool = realloc(pool, ++pool_len * sizeof(bz_auction_pool_t *));
            pool[pool_len-1] = malloc(sizeof(bz_auction_pool_t));
            pool[pool_len-1]->candidates = malloc(sizeof(bz_auction_candidate_t *));
            pool[pool_len-1]->size = 0;
        }

        while (rc == SQLITE_ROW) {
            bid_t max_bid = sqlite3_column_int64(stmt, 4);

            double predicted_profit = predictions[i]->value - max_bid;

            if (predicted_profit < 0) goto next_loop;

            pool[pool_len-1]->candidates = realloc(pool[pool_len-1]->candidates,  ++pool[pool_len-1]->size * sizeof(bz_auction_candidate_t *));

            LAST_CANDIDATE = malloc(sizeof(bz_auction_candidate_t));

            LAST_CANDIDATE->predicted = predictions[i];
            LAST_CANDIDATE->profit = predicted_profit;
            LAST_CANDIDATE->item = malloc(sizeof(bz_auction_item_t));

            const char* uuid = sqlite3_column_text(stmt, 0);

            LAST_CANDIDATE->item->uuid = calloc(sizeof(char), strlen(uuid) + 1);

            strcpy(LAST_CANDIDATE->item->uuid, uuid);

            if (sqlite3_column_type(stmt, 1) == SQLITE_NULL) { 
                LAST_CANDIDATE->item->auctioneer = calloc(sizeof(char), 1);
            } else {
                const char* auctioneer = sqlite3_column_text(stmt, 1);
                LAST_CANDIDATE->item->auctioneer = calloc(sizeof(char), strlen(auctioneer) + 1);

                strcpy(LAST_CANDIDATE->item->auctioneer, auctioneer);
            }

            LAST_CANDIDATE->item->start = sqlite3_column_int64(stmt, 2);
            LAST_CANDIDATE->item->end = sqlite3_column_int64(stmt, 3);
            LAST_CANDIDATE->item->has_bid = sqlite3_column_int(stmt, 5);

            const char* item_full_name = sqlite3_column_text(stmt, 6);

            LAST_CANDIDATE->item->name.full = calloc(sizeof(char), strlen(item_full_name) + 1);

            strcpy(LAST_CANDIDATE->item->name.full, item_full_name);

            const char* item_name = sqlite3_column_text(stmt, 7);

            LAST_CANDIDATE->item->name.item = calloc(sizeof(char), strlen(item_name) + 1);

            strcpy(LAST_CANDIDATE->item->name.item, item_name);

            LAST_CANDIDATE->item->bin = sqlite3_column_int(stmt, 8);
next_loop:
            rc = sqlite3_step(stmt);
        }
        
        sqlite3_finalize(stmt);
    }

    *poollen = pool_len;

    return pool;
}

void bz_free_auction_pool (bz_auction_pool_t **pool, size_t len) {
    for (size_t i = 0; i < len; i++) {
        for (size_t x = 0; x < pool[i]->size; x++) {
            free(pool[i]->candidates[x]->item->name.item);
            free(pool[i]->candidates[x]->item->name.full);
            free(pool[i]->candidates[x]->item->auctioneer);
            free(pool[i]->candidates[x]->item->uuid);
            free(pool[i]->candidates[x]->item);
            free(pool[i]->candidates[x]);
        }
        free(pool[i]->candidates);
        free(pool[i]);
    }
    free(pool);
}

bz_auction_pool_t *bz_random_auction_flips (
    bz_auction_pool_t **pool, 
    size_t len, 
    bid_t max_bid, 
    double min_profit, 
    time_t max_time_left, 
    int min_dataset, 
    size_t try_item_amount, 
    char **ignore_item
) {
    bz_auction_pool_t *ret = malloc(sizeof(bz_auction_pool_t));

    ret->size = 0;
    ret->candidates = malloc(sizeof(bz_auction_candidate_t *));

    if (len < 1) return ret;

    size_t start_index = random_range(0, len - 1);

    size_t i = start_index;

    time_t current_time = get_time_ms();

    while (1) {
        if (++i >= len) i = 0; // loop back

        if (pool[i]->size < 1) continue;

        size_t _start_index = random_range(0, pool[i]->size - 1);

        size_t x = _start_index;
        while (1) {
            if (++x >= pool[i]->size) x = 0;

            if (pool[i]->candidates[x]->item->end < current_time) goto fail_continue;
            if (pool[i]->candidates[x]->item->end - current_time > max_time_left) goto fail_continue;
            if (max_bid && (pool[i]->candidates[x]->item->max_bid > max_bid)) goto fail_continue;
            if (pool[i]->candidates[x]->profit < min_profit) goto fail_continue;
            if (pool[i]->candidates[x]->predicted->n < min_dataset) goto fail_continue;

            ret->candidates = realloc(ret->candidates, sizeof(bz_auction_candidate_t *) * ++ret->size);

            ret->candidates[ret->size-1] = pool[i]->candidates[x];

            if (ret->size == try_item_amount) return ret;

            break;
fail_continue:
            if (x == _start_index) break;
        }
        if (i == start_index) break; // exit when full loop;
    }
    return ret;
}

void bz_free_random_auction_flips (bz_auction_pool_t *pool) {
    free(pool->candidates);
    free(pool);
}