#include "fetch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

static size_t write_cb (char *in, size_t size, size_t nmemb, bz_fetch_res_t *out) {
    size_t realsize = size * nmemb;
    char *ptr = realloc(out->buf, out->len + realsize + 1);
    out->buf = ptr;
    memcpy(&(out->buf[out->len]), in, realsize);
    out->len += realsize;
    out->buf[out->len] = 0;
    return realsize;
}

void fetch_free (bz_fetch_res_t *data) {
    free(data->buf);
    free(data);
};

bz_fetch_res_t *fetch_init_res () {
    bz_fetch_res_t *ret = malloc(sizeof(bz_fetch_res_t));
    ret->buf = malloc(1);
    ret->len = 0;
    return ret;
}

int fetch (char *url, bz_fetch_res_t *data) {
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "fetch failed: Could not init curl");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "bazcal/2.1");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "fetch failed: %s\n", curl_easy_strerror(res));
        return 1;
    }

    curl_easy_cleanup(curl);

    return 0;
}