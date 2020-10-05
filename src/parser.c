#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAST_AUCTION parsed->auctions[parsed->count - 1]

int bz_parse_res (bz_fetch_res_t *res, bz_auction_page_t *parsed) {
    json_t *root;
    json_error_t error;
    root = json_loads(res->buf, 0, &error);

    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }

    if (!json_is_object(root)) {
        fprintf(stderr, "error: response is not an object\n");
        goto fail;
    }

    
    json_t *pages = json_object_get(root, "totalPages");
    json_t *page = json_object_get(root, "page");
    json_t *name = json_object_get(root, "auctions");
    json_t *auctions = json_object_get(root, "auctions");
    
    if (!json_is_integer(page) || !json_is_integer(pages)) {
        goto fail;
    };

    parsed->page = json_integer_value(page);
    parsed->pages = json_integer_value(pages);

    if (!json_is_array(auctions)) {
        fprintf(stderr, "error: auctions is not an array\n");
        goto fail;
    }

    for(size_t i = 0; i < json_array_size(auctions); i++) {
        json_t *auction = json_array_get(auctions, i);
        
        if (!json_is_object(auction)) {
            fprintf(stderr, "error: auction is not an object\n");
            continue; // skip this iteration
        };

        json_t *uuid = json_object_get(auction, "uuid");
        json_t *auctioneer = json_object_get(auction, "auctioneer");
        json_t *start = json_object_get(auction, "start");
        json_t *end = json_object_get(auction, "end");
        json_t *extra = json_object_get(auction, "extra");
        json_t *tier = json_object_get(auction, "tier");
        json_t *item_name = json_object_get(auction, "item_name");
        json_t *bin = json_object_get(auction, "bin");
        json_t *starting_bid = json_object_get(auction, "starting_bid");
        json_t *highest_bid_amount = json_object_get(auction, "highest_bid_amount");

        if (
            !json_is_string(uuid) || !json_is_string(auctioneer) || !json_is_string(extra) || !json_is_string(tier) || !json_is_string(item_name) ||
            !json_is_integer(start) || !json_is_integer(end) || !json_is_integer(starting_bid) || !json_is_integer(highest_bid_amount)
        ) {
            continue; // skip
        };

        parsed->count++;
        bz_auction_item_t **__auctions = realloc(parsed->auctions, parsed->count * sizeof(bz_auction_item_t*));
        parsed->auctions = __auctions;
        LAST_AUCTION = malloc(sizeof(bz_auction_item_t));

        const char *auctioneer_cstr = json_string_value(auctioneer);
        const char *uuid_cstr = json_string_value(uuid);

        LAST_AUCTION->auctioneer = calloc(sizeof(char), strlen(auctioneer_cstr) + 1);
        LAST_AUCTION->uuid = calloc(sizeof(char), strlen(uuid_cstr) + 1);

        strcpy(LAST_AUCTION->uuid, uuid_cstr);
        strcpy(LAST_AUCTION->auctioneer, auctioneer_cstr);

        LAST_AUCTION->start = json_integer_value(start);
        LAST_AUCTION->end = json_integer_value(end);
        LAST_AUCTION->bin = json_is_boolean(bin) ? json_boolean_value(bin) : 0;

        json_int_t higest_bid_int = json_integer_value(highest_bid_amount), start_bid_int = json_integer_value(starting_bid);

        LAST_AUCTION->max_bid = higest_bid_int ? higest_bid_int : start_bid_int;

        const char *item_name_cstr = json_string_value(item_name);

        LAST_AUCTION->name.item = calloc(sizeof(char), strlen(item_name_cstr) + 1);
        strcpy(LAST_AUCTION->name.item, item_name_cstr);

        const char *tier_cstr = json_string_value(tier);

        size_t tier_len = strlen(tier_cstr);

        const char *extra_cstr = json_string_value(extra);

        size_t full_len = strlen(extra_cstr) + tier_len + 2;

        LAST_AUCTION->name.full = calloc(sizeof(char), full_len);

        strcpy(LAST_AUCTION->name.full, tier_cstr);

        LAST_AUCTION->name.full[tier_len] = ' ';

        strcpy(&LAST_AUCTION->name.full[tier_len + 1], extra_cstr);

        LAST_AUCTION->name.full[full_len - 1] = 0;
    }
    json_decref(root);
    return 0;
fail:
    json_decref(root);
    return 1;
}

bz_auction_page_t *bz_auction_page_init () {
    bz_auction_page_t *res = malloc(sizeof(bz_auction_page_t));

    res->count = 0;
    res->page = 0;
    res->pages = 0;
    res->auctions = malloc(sizeof(bz_auction_page_t*));
}

void auction_page_free (bz_auction_page_t *page) {
    for (size_t c = 0; c < page->count; c++) {
        free(page->auctions[c]->name.item);
        free(page->auctions[c]->name.full);
        free(page->auctions[c]->auctioneer);
        free(page->auctions[c]->uuid);
        free(page->auctions[c]);
        page->auctions[c] = NULL;
    };
    free(page->auctions);
    free(page);
}