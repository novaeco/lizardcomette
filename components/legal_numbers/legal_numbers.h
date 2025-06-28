#ifndef LEGAL_NUMBERS_H
#define LEGAL_NUMBERS_H

#include <stdbool.h>

#define LEGAL_NUMBERS_MAX 100

typedef struct {
    int id;
    char username[32];
    int elevage_id; /* 0 for any */
    char type[4];   /* "CDC" or "AOE" */
    char number[32];
} LegalNumber;

void legal_numbers_init(void);
bool legal_numbers_add(const LegalNumber *n);
const LegalNumber *legal_numbers_get(int id);
bool legal_numbers_update(int id, const LegalNumber *n);
bool legal_numbers_delete(int id);
int legal_numbers_count(void);
const LegalNumber *legal_numbers_get_by_index(int index);
bool legal_numbers_is_cdc_valid(const char *number, const char *username, int elevage_id);
bool legal_numbers_is_aoe_valid(const char *number, const char *username, int elevage_id);

#endif // LEGAL_NUMBERS_H
