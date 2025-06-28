#ifndef BREEDING_H
#define BREEDING_H

#include <stdbool.h>

#define BREEDING_MAX 100

typedef struct {
    int id;
    int animal_id;
    char description[64];
    int date; /* timestamp */
} BreedingEvent;

void breeding_init(void);
bool breeding_add(const BreedingEvent *ev);
const BreedingEvent *breeding_get(int id);
bool breeding_update(int id, const BreedingEvent *ev);
bool breeding_delete(int id);
int breeding_count(void);
const BreedingEvent *breeding_get_by_index(int index);
int breeding_count_for_animal(int animal_id);
const BreedingEvent *breeding_get_by_index_for_animal(int index, int animal_id);

#endif // BREEDING_H
