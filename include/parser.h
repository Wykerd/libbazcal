#ifndef BZ_PARSER_H
#define BZ_PARSER_H

#include "bazcal.h"
#include "fetch.h"
#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bz_auction_page_s {
    json_int_t page;
    json_int_t pages;
    bz_auction_item_t **auctions;
    size_t count;
};

typedef struct bz_auction_page_s bz_auction_page_t;

void auction_page_free (bz_auction_page_t *page);
int bz_parse_auctions (bz_fetch_res_t *res, bz_auction_page_t *parsed);
bz_auction_page_t *bz_auction_page_init ();

struct bz_bazaar_s {
    size_t count;
    bz_bazaar_item_t **items;
};

typedef struct bz_bazaar_s bz_bazaar_t;

void bazaar_free (bz_bazaar_t *data);
int bz_parse_bazaar (bz_fetch_res_t *res, bz_bazaar_t *parsed);
bz_bazaar_t *bz_bazaar_init ();

#ifdef __cplusplus
}
#endif

#endif