#ifndef HEALTH_H
#define HEALTH_H

#include <stdbool.h>

#define HEALTH_MAX 100

typedef struct {
    int id;
    int animal_id;
    char description[64];
    int date; /* timestamp */
} HealthRecord;

void health_init(void);
bool health_add(const HealthRecord *rec);
const HealthRecord *health_get(int id);
bool health_update(int id, const HealthRecord *rec);
bool health_delete(int id);
int health_count(void);
const HealthRecord *health_get_by_index(int index);
int health_count_for_animal(int animal_id);
const HealthRecord *health_get_by_index_for_animal(int index, int animal_id);

#endif // HEALTH_H
