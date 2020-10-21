#include "db.h"

int bz_init_db (sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare(db, "CREATE TABLE IF NOT EXISTS auctions ("
        "uuid TEXT PRIMARY KEY,"
        "auctioneer TEXT,"
        "start INTEGER NOT NULL,"
        "end INTEGER NOT NULL,"
        "max_bid INTEGER NOT NULL,"
        "has_bid INTEGER NOT NULL,"
        "item_full_name TEXT NOT NULL,"
        "item_name TEXT NOT NULL)", -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "DB init prepare error: %s\n", sqlite3_errmsg(db));

        return rc;
    }

    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return rc;
};

int bz_add_auc_record (sqlite3 *db, bz_auction_item_t *item) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare(db, "REPLACE INTO auctions ( uuid, auctioneer, start, end, max_bid, item_full_name, item_name, has_bid ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ? )", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "DB auction record replace prepare error: %s\n", sqlite3_errmsg(db));

        return rc;
    };

    CHECK_BIND_OK(sqlite3_bind_text(stmt, 1, item->uuid, -1, NULL));
    CHECK_BIND_OK(sqlite3_bind_text(stmt, 2, item->auctioneer, -1, NULL));
    CHECK_BIND_OK(sqlite3_bind_int64(stmt, 3, item->start));
    CHECK_BIND_OK(sqlite3_bind_int64(stmt, 4, item->end));
    CHECK_BIND_OK(sqlite3_bind_int64(stmt, 5, item->max_bid));
    CHECK_BIND_OK(sqlite3_bind_text(stmt, 6, item->name.full, -1, NULL));
    CHECK_BIND_OK(sqlite3_bind_text(stmt, 7, item->name.item, -1, NULL));
    CHECK_BIND_OK(sqlite3_bind_int64(stmt, 8, item->has_bid));

    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return rc;
};

int bz_add_auc_page (sqlite3 *db, bz_auction_page_t *page) {
    for (size_t i = 0; i < page->count; i++) {
        bz_add_auc_record(db, page->auctions[i]);
    }
};