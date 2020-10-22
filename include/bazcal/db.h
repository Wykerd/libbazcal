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

#ifndef BZ_DB_H
#define BZ_DB_H

#include "bazcal.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

int bz_init_db (sqlite3 *db);

int bz_add_auc_record (sqlite3 *db, bz_auction_item_t *item);

int bz_add_auc_page (sqlite3 *db, bz_auction_page_t *page);

#ifdef __cplusplus
}
#endif

#endif