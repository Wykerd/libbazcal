#include "bazcal.h"
#include "fetch.h"
#include "parser.h"
#include <stdio.h>
#include <curl/curl.h>

int main () {
    bz_fetch_res_t *res = fetch_init_res();
    fetch("https://api.hypixel.net/skyblock/auctions", res);
    bz_auction_page_t *parsed = bz_auction_page_init();
    bz_parse_res(res, parsed);
    for (size_t i = 0; i < parsed->count; i++) {

        printf("NAME: %s\nCOUNT: %d\nUUID: %s\n\n", parsed->auctions[i]->name.full, parsed->count, parsed->auctions[i]->uuid);
    }
    auction_page_free(parsed);
    fetch_free(res);
}