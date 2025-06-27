#ifndef ANIMALS_H
#define ANIMALS_H

#include <stdbool.h>

#define ANIMALS_MAX 50

/**
 * \brief Informations sur un reptile.
 */
typedef struct {
    int id;
    char name[32];
    char species[32];
    char sex[8];
    char birth_date[16];
    char health[64];
    char breeding_cycle[64];
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

#endif // ANIMALS_H
