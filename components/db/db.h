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

#endif // DB_H
