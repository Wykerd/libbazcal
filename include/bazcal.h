#ifndef BZ_BAZCAL_H
#define BZ_BAZCAL_H

#include <stddef.h>
#include <time.h>

#define UUID_LENGTH 32

typedef __uint8_t* uuid_t;
typedef __uint64_t bid_t;

struct bz_bid_info_s {
    bid_t max;
    time_t time;
};

typedef struct bz_bid_info_s bz_bid_info_t;

struct bz_auction_item_s {
    uuid_t uuid; // 128 bit unique id
    uuid_t auctioneer;
    time_t start;
    time_t end;
    struct {
        char *full;
        char *item;
    } name;
    bid_t max_bid;
    int bin;
};

typedef struct bz_auction_item_s bz_auction_item_t;

struct bz_auction_prediction_s {
    bid_t value;
    bz_bid_info_t last_auction;
    size_t n;
};

typedef struct bz_auction_prediction_s bz_auction_prediction_t;

#endif