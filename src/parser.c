#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAST_AUCTION parsed->auctions[parsed->count - 1]
#define LAST_ITEM parsed->items[parsed->count - 1]

int bz_parse_auctions (bz_fetch_res_t *res, bz_auction_page_t *parsed) {
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

        LAST_AUCTION->has_bid = higest_bid_int ? 1 : 0;

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

    return res;
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


int bz_parse_bazaar (bz_fetch_res_t *res, bz_bazaar_t *parsed) {
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

    json_t *products = json_object_get(root, "products");

    if (!json_is_object(products)) {
        fprintf(stderr, "error: products is not an object\n");
        goto fail;
    }

    const char *p_key;
    json_t *product;

    json_object_foreach(products, p_key, product) {
        if (!json_is_object(product)) {
            fprintf(stderr, "error: product is not an object\n");
            continue; // skip this iteration
        };

        json_t *name_j = json_object_get(product, "product_id");

        json_t *sell_summary_j = json_object_get(product, "sell_summary");
        if (!json_is_array(sell_summary_j)) continue;
        if (json_array_size(sell_summary_j) == 0) continue;
        json_t *sell_summary_0_j = json_array_get(sell_summary_j, 0);
        if (!json_is_object(sell_summary_0_j)) continue;
        json_t *buy_j = json_object_get(sell_summary_0_j, "pricePerUnit");

        json_t *buy_summary_j = json_object_get(product, "buy_summary");
        if (!json_is_array(buy_summary_j)) continue;
        if (json_array_size(buy_summary_j) == 0) continue;
        json_t *buy_summary_0_j = json_array_get(buy_summary_j, 0);
        if (!json_is_object(buy_summary_0_j)) continue;
        json_t *sell_j = json_object_get(buy_summary_0_j, "pricePerUnit");

        json_t *quick_status_j = json_object_get(product, "quick_status");
        if (!json_is_object(quick_status_j)) continue;

        json_t *volume_j = json_object_get(quick_status_j, "buyMovingWeek");
        json_t *svolume_j = json_object_get(quick_status_j, "sellMovingWeek");

        if (!json_is_number(buy_j) || !json_is_number(sell_j) || !json_is_integer(volume_j) || !json_is_integer(svolume_j) || !json_is_string(name_j)) continue;

        parsed->count++;
        parsed->items = realloc(parsed->items, parsed->count * sizeof(bz_bazaar_item_t*));
        LAST_ITEM  = malloc(sizeof(bz_bazaar_item_t));

        const char *item_name = json_string_value(name_j);

        LAST_ITEM->item_name = calloc(sizeof(char), strlen(item_name) + 1);

        strcpy(LAST_ITEM->item_name, item_name);

        LAST_ITEM->sell = json_number_value(sell_j);
        LAST_ITEM->buy = json_number_value(buy_j);

        LAST_ITEM->volume = json_integer_value(volume_j);
        LAST_ITEM->svolume = json_integer_value(svolume_j);
    }

    json_decref(root);
    return 0;
fail:
    json_decref(root);
    return 1;
}

void bazaar_free (bz_bazaar_t *data) {
    for (size_t c = 0; c < data->count; c++) {
        free(data->items[c]->item_name);
        free(data->items[c]);
        data->items[c] = NULL;
    };
    free(data->items);
    free(data);
}

bz_bazaar_t *bz_bazaar_init () {
    bz_bazaar_t *res = malloc(sizeof(bz_bazaar_t));

    res->count = 0;
    res->items = malloc(sizeof(bz_bazaar_item_t*));

    return res;
}