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