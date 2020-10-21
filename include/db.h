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