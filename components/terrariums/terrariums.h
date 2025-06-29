#ifndef TERRARIUMS_H
#define TERRARIUMS_H

#include <stdbool.h>

#define TERRARIUMS_MAX 20
#define LOGS_MAX 100

/**
 * \brief Informations sur un terrarium.
 */
typedef struct {
    int id;
    int elevage_id;
    char name[32];
    int capacity;
    char inventory[64];
    char notes[64];
} Terrarium;

/**
 * \brief Initialise la liste des terrariums et charge les logs.
 *
 * \param log_offset Decalage dans l'historique des logs pour la pagination.
 *                   Utiliser 0 pour recuperer les dernieres entrees.
 */
void terrariums_init(int log_offset);

/**
 * \brief Ajoute un terrarium.
 */
bool terrariums_add(const Terrarium *t);

/**
 * \brief Récupère un terrarium par id.
 */
const Terrarium *terrariums_get(int id);

/**
 * \brief Met à jour un terrarium existant.
 */
bool terrariums_update(int id, const Terrarium *t);

/**
 * \brief Supprime un terrarium par id.
 */
bool terrariums_delete(int id);

/**
 * \brief Enregistre une transaction ou un événement dans les logs.
 */
void terrariums_log_transaction(const char *msg);

int terrariums_count_for_elevage(int elevage_id);
const Terrarium *terrariums_get_by_index_for_elevage(int index, int elevage_id);

#endif // TERRARIUMS_H
