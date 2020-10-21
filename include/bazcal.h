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


struct bz_auction_pool_s {
    bz_auction_candidate_t **candidates;
    size_t size;
};

typedef struct bz_auction_pool_s bz_auction_pool_t;

#ifdef __cplusplus
}
#endif

#endif