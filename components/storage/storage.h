#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

/**
 * \brief Initialise le stockage externe (SD, SPIFFS...).
 */
void storage_init(void);

/**
 * \brief Export des données en CSV ou JSON.
 */
void storage_export(void);

/**
 * \brief Chiffre un fichier de stockage local.
 * \param path Chemin du fichier à chiffrer.
 */
bool storage_encrypt_file(const char *path);

#endif // STORAGE_H
