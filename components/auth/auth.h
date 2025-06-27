#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

/**
 * \brief Rôles utilisateurs disponibles.
 */
typedef enum {
    ROLE_PARTICULIER,  /**< Accès utilisateur particulier */
    ROLE_PROFESSIONNEL /**< Accès utilisateur professionnel */
} user_role_t;

/**
 * \brief Initialise le système d'authentification.
 */
void auth_init(void);

/**
 * \brief Définit les informations d'identification (hashage interne).
 */
void auth_set_credentials(const char *password, user_role_t role);

/**
 * \brief Vérifie le mot de passe utilisateur.
 */
bool auth_check(const char *password);

/**
 * \brief Récupère le rôle de l'utilisateur courant.
 */
user_role_t auth_get_role(void);

#endif // AUTH_H
