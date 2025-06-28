#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include "animals.h"

/**
 * \brief Initialise le stockage externe et charge la cle AES.
 */
void storage_init(void);

/**
 * \brief Monte une carte SD sur /sdcard.
 * \return true en cas de succes.
 */
bool storage_mount_sdcard(void);

/**
 * \brief Exporte toutes les donnees de la base au format CSV.
 * \param path Chemin du fichier de sortie.
 */
void storage_export_csv(const char *path);

/**
 * \brief Exporte les documents legaux au format PDF.
 * \param dir Repertoire de destination des PDF.
 * \param r   Reptile concerne par les documents.
 */
void storage_export_pdf(const char *dir, const Reptile *r);

/**
 * \brief Transfere un fichier via le Wi-Fi.
 * \param path Chemin du fichier a envoyer.
 * \param url  URL de destination.
 */
bool storage_wifi_transfer(const char *path, const char *url);

/**
 * \brief Restaure la base de donnees a partir du fichier de sauvegarde.
 */
void storage_restore(void);

/**
 * \brief Chiffre un fichier de stockage local.
 * \param path Chemin du fichier à chiffrer.
 */
bool storage_encrypt_file(const char *path);

/**
 * \brief Dechiffre un fichier chiffre vers une nouvelle destination.
 * \param src Fichier chiffre source.
 * \param dst Fichier de sortie dechiffre.
 */
bool storage_decrypt_file(const char *src, const char *dst);

/**
 * \brief Definit la cle AES utilisee pour le chiffrement.
 * \param key Tableau de 16 octets.
 * \return true si la cle a ete enregistree.
 */
bool storage_set_aes_key(const unsigned char key[16]);

#endif // STORAGE_H
