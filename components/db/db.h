#ifndef DB_H
#define DB_H

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

#endif // DB_H
