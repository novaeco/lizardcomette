#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <stdbool.h>

#define TRANSACTIONS_MAX 100

typedef enum {
    TRANSACTION_PURCHASE,
    TRANSACTION_SALE
} transaction_type_t;

typedef struct {
    int id;
    int stock_id;
    int quantity;
    transaction_type_t type;
} Transaction;

void transactions_init(void);
bool transactions_add(const Transaction *t);
const Transaction *transactions_get(int id);
bool transactions_update(int id, const Transaction *t);
bool transactions_delete(int id);

#endif // TRANSACTIONS_H
