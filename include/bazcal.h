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

#ifndef BZ_BAZCAL_H
#define BZ_BAZCAL_H

#include <stddef.h>
#include <time.h>
#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHECK_BIND_OK(x) rc = x; if (rc != SQLITE_OK) { fprintf(stderr, "DB bind error: %s\n", sqlite3_errmsg(db)); return rc; };
#define CHECK_BIND_OK_NULL(x) if (x != SQLITE_OK) { fprintf(stderr, "DB bind error: %s\n", sqlite3_errmsg(db)); return NULL; };

#define UUID_LENGTH 32

typedef char* uuid_t;
typedef __uint64_t bid_t;
typedef __uint32_t uint32_t;

struct bz_auction_item_s {
    uuid_t uuid;
    uuid_t auctioneer;
    time_t start;
    time_t end;
    struct {
        char *full;
        char *item;
    } name;
    bid_t max_bid;
    int bin;
    int has_bid;
};

typedef struct bz_auction_item_s bz_auction_item_t;

struct bz_bazaar_item_s {
    char* item_name;
    double buy;
    double sell;
    int volume;
    int svolume;
};

typedef struct bz_bazaar_item_s bz_bazaar_item_t;

struct bz_prediction_s {
    char* item_name;
    size_t n;
    double value;
};

typedef struct bz_prediction_s bz_prediction_t;

struct bz_auction_candidate_s {
    bz_auction_item_t *item;
    bz_prediction_t *predicted;
    double profit;
};

typedef struct bz_auction_candidate_s bz_auction_candidate_t;

struct bz_bazaar_s {
    size_t count;
    bz_bazaar_item_t **items;
};

typedef struct bz_bazaar_s bz_bazaar_t;

struct bz_auction_pool_s {
    bz_auction_candidate_t **candidates;
    size_t size;
};

typedef struct bz_auction_pool_s bz_auction_pool_t;

bz_auction_pool_t **bz_populate_auction_pool (sqlite3 *db, bz_prediction_t **predictions, size_t len, size_t *poollen);

bz_auction_pool_t *bz_random_auction_flips (
    bz_auction_pool_t **pool, 
    size_t len, 
    double max_bid, 
    double min_profit, 
    time_t max_time_left, 
    time_t min_time_left,
    int min_dataset, 
    size_t try_item_amount, 
    char **ignore_item
);

void bz_free_auction_pool (bz_auction_pool_t **pool, size_t len);
void bz_free_random_auction_flips (bz_auction_pool_t *pool);

bz_prediction_t **bz_generate_predictions (sqlite3 *db, size_t *len);

void bz_free_predictions (bz_prediction_t **predictions, size_t len);

void bz_auction_loop (const char* database_name, int log_level, void (*cycle_callback)(sqlite3 *db));

struct bz_bazaar_advise_s {
    char* name;
    double evolume;
    double invested;
    double eprofit;
    double pinvested;
    double pprofit;
};

typedef struct bz_bazaar_advise_s bz_bazaar_advise_t;

struct bz_bazaar_advice_buf_s {
    bz_bazaar_advise_t **items;
    size_t count;
};

typedef struct bz_bazaar_advice_buf_s bz_bazaar_advice_buf_t;

bz_bazaar_advice_buf_t *bz_advise (bz_bazaar_t *data, bid_t balance, uint32_t time);

void bz_free_advise (bz_bazaar_advice_buf_t *advise);

#ifdef __cplusplus
}
#endif

#endif