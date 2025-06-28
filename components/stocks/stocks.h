#ifndef STOCKS_H
#define STOCKS_H

#include <stdbool.h>

#define STOCKS_MAX 50

typedef struct {
    int id;
    char name[32];
    int quantity;
    int min_quantity;
} StockItem;

void stocks_init(void);
bool stocks_add(const StockItem *item);
const StockItem *stocks_get(int id);
bool stocks_update(int id, const StockItem *item);
bool stocks_delete(int id);
int stocks_count(void);
const StockItem *stocks_get_by_index(int index);

#endif // STOCKS_H
