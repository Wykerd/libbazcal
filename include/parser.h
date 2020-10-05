#ifndef BZ_PARSER_H
#define BZ_PARSER_H

#include "bazcal.h"
#include "fetch.h"
#include <jansson.h>

struct bz_auction_page_s {
    json_int_t page;
    json_int_t pages;
    bz_auction_item_t **auctions;
    size_t count;
};

typedef struct bz_auction_page_s bz_auction_page_t;

void auction_page_free (bz_auction_page_t *page);

int bz_parse_res (bz_fetch_res_t *res, bz_auction_page_t *parsed);
bz_auction_page_t *bz_auction_page_init ();

#endif