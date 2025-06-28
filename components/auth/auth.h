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

#define AUTH_MAX_USERS 5

typedef struct {
    char username[32];
    unsigned char hash[32];
    user_role_t role;
} auth_user_t;

/**
 * \brief Initialise le système d'authentification.
 */
void auth_init(void);

/**
 * \brief Ajoute un utilisateur avec mot de passe haché.
 */
bool auth_add_user(const char *username, const char *password, user_role_t role);

/**
 * \brief Vérifie le mot de passe utilisateur.
 */
bool auth_check(const char *username, const char *password);

/**
 * \brief Récupère le rôle d'un utilisateur.
 */
user_role_t auth_get_role(const char *username);

#endif // AUTH_H
