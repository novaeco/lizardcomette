#ifndef ELEVAGES_H
#define ELEVAGES_H

#include <stdbool.h>

#define ELEVAGES_MAX 10

typedef struct {
    int id;
    char name[32];
    char description[64];
} Elevage;

void elevages_init(void);
bool elevages_add(const Elevage *e);
const Elevage *elevages_get(int id);
bool elevages_update(int id, const Elevage *e);
bool elevages_delete(int id);
int elevages_count(void);
const Elevage *elevages_get_by_index(int index);

#endif // ELEVAGES_H
