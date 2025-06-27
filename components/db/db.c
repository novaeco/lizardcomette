#include "db.h"
#include "esp_log.h"

static const char *TAG = "db";

void db_init(void)
{
    ESP_LOGI(TAG, "Initialisation de la base de données");
    // TODO: implémenter l'initialisation SQLite ou système de fichiers
}

void db_backup(void)
{
    ESP_LOGI(TAG, "Sauvegarde de la base de données");
    // TODO: implémenter la sauvegarde sur SD ou Wi-Fi
}
