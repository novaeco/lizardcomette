#include "transactions.h"
#include "esp_log.h"
#include "db.h"
#include "animals.h"
#include "legal.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "transactions";

static Transaction transactions[TRANSACTIONS_MAX];
static int transaction_count = 0;

void transactions_init(void)
{
    transaction_count = 0;
    memset(transactions, 0, sizeof(transactions));

    sqlite3_stmt *stmt = db_query("SELECT id,stock_id,quantity,type FROM transactions;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && transaction_count < TRANSACTIONS_MAX) {
            Transaction *tr = &transactions[transaction_count++];
            tr->id = sqlite3_column_int(stmt, 0);
            tr->stock_id = sqlite3_column_int(stmt, 1);
            tr->quantity = sqlite3_column_int(stmt, 2);
            const char *type = (const char *)sqlite3_column_text(stmt, 3);
            tr->type = (type && strcmp(type, "SALE") == 0) ? TRANSACTION_SALE : TRANSACTION_PURCHASE;
        }
        sqlite3_finalize(stmt);
    }

    ESP_LOGI(TAG, "Initialisation des transactions");
}

static int find_index(int id)
{
    for (int i = 0; i < transaction_count; ++i) {
        if (transactions[i].id == id)
            return i;
    }
    return -1;
}

bool transactions_add(const Transaction *t)
{
    if (transaction_count >= TRANSACTIONS_MAX || !t)
        return false;
    const char *type = t->type == TRANSACTION_SALE ? "SALE" : "PURCHASE";
    sqlite3_stmt *stmt =
        db_prepare("INSERT INTO transactions(id,stock_id,quantity,type) VALUES(?,?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, t->id);
    sqlite3_bind_int(stmt, 2, t->stock_id);
    sqlite3_bind_int(stmt, 3, t->quantity);
    sqlite3_bind_text(stmt, 4, type, -1, SQLITE_TRANSIENT);
    if (!db_step_finalize(stmt))
        return false;
    transactions[transaction_count] = *t;
    transaction_count++;
    ESP_LOGI(TAG, "Ajout de la transaction %d", t->id);

    if (t->type == TRANSACTION_SALE) {
        const Reptile *r = animals_get(t->stock_id);
        if (r) {
            char path[32];
            snprintf(path, sizeof(path), "invoice_%d.pdf", t->id);
            legal_generate_invoice(path, r);
        }
    }
    return true;
}

const Transaction *transactions_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &transactions[idx];
    return NULL;
}

bool transactions_update(int id, const Transaction *t)
{
    int idx = find_index(id);
    if (idx < 0 || !t)
        return false;
    const char *type = t->type == TRANSACTION_SALE ? "SALE" : "PURCHASE";
    sqlite3_stmt *stmt = db_prepare(
        "UPDATE transactions SET stock_id=?,quantity=?,type=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, t->stock_id);
    sqlite3_bind_int(stmt, 2, t->quantity);
    sqlite3_bind_text(stmt, 3, type, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, id);
    if (!db_step_finalize(stmt))
        return false;
    transactions[idx] = *t;
    ESP_LOGI(TAG, "Mise a jour de la transaction %d", id);
    return true;
}

bool transactions_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_prepare("DELETE FROM transactions WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    if (!db_step_finalize(stmt))
        return false;
    for (int i = idx; i < transaction_count - 1; ++i) {
        transactions[i] = transactions[i + 1];
    }
    transaction_count--;
    ESP_LOGI(TAG, "Suppression de la transaction %d", id);
    return true;
}

int transactions_count(void)
{
    return transaction_count;
}

const Transaction *transactions_get_by_index(int index)
{
    if (index < 0 || index >= transaction_count)
        return NULL;
    return &transactions[index];
}

