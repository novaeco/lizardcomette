#include "transactions.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "transactions";

static Transaction transactions[TRANSACTIONS_MAX];
static int transaction_count = 0;

void transactions_init(void)
{
    transaction_count = 0;
    memset(transactions, 0, sizeof(transactions));
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
    transactions[transaction_count] = *t;
    transaction_count++;
    ESP_LOGI(TAG, "Ajout de la transaction %d", t->id);
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
    transactions[idx] = *t;
    ESP_LOGI(TAG, "Mise a jour de la transaction %d", id);
    return true;
}

bool transactions_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    for (int i = idx; i < transaction_count - 1; ++i) {
        transactions[i] = transactions[i + 1];
    }
    transaction_count--;
    ESP_LOGI(TAG, "Suppression de la transaction %d", id);
    return true;
}

