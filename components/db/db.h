#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <stdbool.h>

/**
 * \brief Initialise la base de données locale.
 */
void db_init(void);

/**
 * \brief Sauvegarde la base de données sur stockage externe.
 */
void db_backup(void);

/**
 * \brief Exporte toutes les tables au format CSV.
 *
 * \param path Chemin du fichier de destination.
 */
void db_export_csv(const char *path);

/**
 * \brief Exporte toutes les tables au format JSON.
 *
 * \param path Chemin du fichier de destination.
 */
void db_export_json(const char *path);

/**
 * \brief Execute une requete SQL sans retour de resultats.
 */
bool db_exec(const char *format, ...);

/**
 * \brief Prepare une requete SELECT et renvoie l'objet sqlite3_stmt.
 */
sqlite3_stmt *db_query(const char *format, ...);

/**
 * \brief Définit la clé SQLCipher à utiliser pour chiffrer la base.
 * \param key Chaîne de caractères UTF-8.
 * \return true si la clé a été enregistrée.
 */
bool db_set_key(const char *key);

#endif // DB_H
