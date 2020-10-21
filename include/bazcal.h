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

typedef __uint8_t* uuid_t;
typedef __uint64_t bid_t;

struct bz_bid_info_s {
    bid_t max;
    time_t time;
};

typedef struct bz_bid_info_s bz_bid_info_t;

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

struct bz_auction_prediction_s {
    bid_t value;
    bz_bid_info_t last_auction;
    size_t n;
};

typedef struct bz_auction_prediction_s bz_auction_prediction_t;

struct bz_bazaar_item_s {
    char* item_name;
    double buy;
    double sell;
    int volume;
    int svolume;
};

typedef struct bz_bazaar_item_s bz_bazaar_item_t;

#ifdef __cplusplus
}
#endif

#endif