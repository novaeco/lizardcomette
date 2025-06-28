#include "stocks.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "stocks";

static StockItem stock_items[STOCKS_MAX];
static int stock_count = 0;

void stocks_init(void)
{
    stock_count = 0;
    memset(stock_items, 0, sizeof(stock_items));
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
    stock_items[idx] = *item;
    ESP_LOGI(TAG, "Mise a jour de l'article %d", id);
    return true;
}

bool stocks_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
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

