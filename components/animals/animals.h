#ifndef ANIMALS_H
#define ANIMALS_H

#include <stdbool.h>

#define ANIMALS_MAX 50

/**
 * \brief Informations sur un reptile.
 */
typedef struct {
    int id;
    int elevage_id;
    char name[32];
    char species[32];
    char sex[8];
    char birth_date[16];
    char health[64];
    char breeding_cycle[64];
    char cdc_number[32];
    char aoe_number[32];
    char ifap_id[32];
    int quota_limit;
    int quota_used;
    int cerfa_valid_until;
    int cites_valid_until;
} Reptile;

/**
 * \brief Initialise la liste des reptiles.
 */
void animals_init(void);

/**
 * \brief Ajoute un reptile.
 */
bool animals_add(const Reptile *r);

/**
 * \brief Récupère un reptile par id.
 */
const Reptile *animals_get(int id);

/**
 * \brief Met à jour un reptile existant.
 */
bool animals_update(int id, const Reptile *r);

/**
 * \brief Supprime un reptile par id.
 */
bool animals_delete(int id);

int animals_count(void);
const Reptile *animals_get_by_index(int index);

int animals_count_for_elevage(int elevage_id);
const Reptile *animals_get_by_index_for_elevage(int index, int elevage_id);

#endif // ANIMALS_H
