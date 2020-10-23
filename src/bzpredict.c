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

#include "bazcal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LIMIT(val, min, max) (((val) < (min)) ? (min) : ((val) > (max) ? (max) : (val)))

bz_bazaar_advice_buf_t *bz_advise (bz_bazaar_t *data, bid_t balance, uint32_t time) {
    bz_bazaar_advice_buf_t *unsorted = malloc(sizeof (bz_bazaar_advice_buf_t));

    unsorted->count = 0;
    unsorted->items = malloc(sizeof(bz_bazaar_advise_t *));

    for (size_t i = 0; i < data->count; i++) {
        // data->items[i]
        unsorted->items = realloc(unsorted->items, ++unsorted->count * sizeof(bz_bazaar_advise_t *));

        unsorted->items[unsorted->count - 1] = malloc(sizeof(bz_bazaar_advise_t));

        unsorted->items[unsorted->count - 1]->name = data->items[i]->item_name;

        double profit = (data->items[i]->sell * 0.99) - data->items[i]->buy;

        uint32_t tvolume = (MIN(data->items[i]->volume, data->items[i]->svolume) / 10080) * time;

        double evolume = LIMIT(tvolume, 0, balance / data->items[i]->buy);

        unsorted->items[unsorted->count - 1]->evolume = evolume;
        unsorted->items[unsorted->count - 1]->invested = data->items[i]->buy * profit;
        unsorted->items[unsorted->count - 1]->eprofit = evolume * profit;
        unsorted->items[unsorted->count - 1]->pinvested = (data->items[i]->buy * evolume) / balance;
        unsorted->items[unsorted->count - 1]->pprofit = profit / data->items[i]->buy;
    };

    for (size_t i = 0; i < unsorted->count - 1; i++) {
        for (size_t x = i + 1; x < unsorted->count; x++) {
            if (unsorted->items[i]->eprofit < unsorted->items[x]->eprofit) {
                bz_bazaar_advise_t *tmp = unsorted->items[i];
                unsorted->items[i] = unsorted->items[x];
                unsorted->items[x] = tmp;
            }
        }
    };

    return unsorted;
};

void bz_free_advise (bz_bazaar_advice_buf_t *advise) {
    for (size_t i = 0; i < advise->count; i++) {
        free(advise->items[i]);
    }
    free(advise->items);
    free(advise);
};