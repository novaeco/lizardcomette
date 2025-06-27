#ifndef AUTH_H
#define AUTH_H

/**
 * \brief Initialise le système d'authentification.
 */
void auth_init(void);

/**
 * \brief Vérifie le mot de passe utilisateur.
 */
bool auth_check(const char *password);

#endif // AUTH_H
