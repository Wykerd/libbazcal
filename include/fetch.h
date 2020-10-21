#ifndef BZ_FETCH_H
#define BZ_FETCH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bz_fetch_res_s {
    char *buf;
    size_t len;
};

typedef struct bz_fetch_res_s bz_fetch_res_t;

// CURL abstraction layer
int fetch (char *url, bz_fetch_res_t *data);
bz_fetch_res_t *fetch_init_res ();
void fetch_free (bz_fetch_res_t *data);

#ifdef __cplusplus
}
#endif

#endif