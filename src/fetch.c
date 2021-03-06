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

#include "bazcal/fetch.h"
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
        fprintf(stderr, "fetch failed: Could not init curl\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "bazcal/3.0");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "fetch failed: %s\n", curl_easy_strerror(res));
        return 1;
    }

    curl_easy_cleanup(curl);

    return 0;
}