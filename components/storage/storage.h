#ifndef STORAGE_H
#define STORAGE_H

/**
 * \brief Initialise le stockage externe (SD, SPIFFS...).
 */
void storage_init(void);

/**
 * \brief Export des données en CSV ou JSON.
 */
void storage_export(void);

#endif // STORAGE_H
