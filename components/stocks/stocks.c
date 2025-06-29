#include "stocks.h"
#include "esp_log.h"
#include "db.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "stocks";

static StockItem stock_items[STOCKS_MAX];
static int stock_count = 0;

void stocks_init(void)
{
    stock_count = 0;
    memset(stock_items, 0, sizeof(stock_items));

    sqlite3_stmt *stmt = db_query("SELECT id,name,quantity,min_quantity FROM stocks;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && stock_count < STOCKS_MAX) {
            StockItem *s = &stock_items[stock_count++];
            s->id = sqlite3_column_int(stmt, 0);
            strncpy(s->name, (const char *)sqlite3_column_text(stmt, 1), sizeof(s->name) - 1);
            s->quantity = sqlite3_column_int(stmt, 2);
            s->min_quantity = sqlite3_column_int(stmt, 3);
        }
        sqlite3_finalize(stmt);
    }

    ESP_LOGI(TAG, "Initialisation des stocks");
}

static int find_index(int id)
{
    for (int i = 0; i < stock_count; ++i) {
        if (stock_items[i].id == id)
            return i;
    }
    return -1;
}

bool stocks_add(const StockItem *item)
{
    if (stock_count >= STOCKS_MAX || !item)
        return false;
    sqlite3_stmt *stmt =
        db_query("INSERT INTO stocks(id,name,quantity,min_quantity) VALUES(?,?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, item->id);
    sqlite3_bind_text(stmt, 2, item->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, item->quantity);
    sqlite3_bind_int(stmt, 4, item->min_quantity);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    stock_items[stock_count] = *item;
    stock_count++;
    ESP_LOGI(TAG, "Ajout de l'article %d", item->id);
    return true;
}

const StockItem *stocks_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &stock_items[idx];
    return NULL;
}

bool stocks_update(int id, const StockItem *item)
{
    int idx = find_index(id);
    if (idx < 0 || !item)
        return false;
    sqlite3_stmt *stmt =
        db_query("UPDATE stocks SET name=?,quantity=?,min_quantity=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_text(stmt, 1, item->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, item->quantity);
    sqlite3_bind_int(stmt, 3, item->min_quantity);
    sqlite3_bind_int(stmt, 4, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    stock_items[idx] = *item;
    ESP_LOGI(TAG, "Mise a jour de l'article %d", id);
    return true;
}

bool stocks_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_query("DELETE FROM stocks WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    for (int i = idx; i < stock_count - 1; ++i) {
        stock_items[i] = stock_items[i + 1];
    }
    stock_count--;
    ESP_LOGI(TAG, "Suppression de l'article %d", id);
    return true;
}

int stocks_count(void)
{
    return stock_count;
}

const StockItem *stocks_get_by_index(int index)
{
    if (index < 0 || index >= stock_count)
        return NULL;
    return &stock_items[index];
}

