#include "storage.h"
#include "esp_log.h"

static const char *TAG = "storage";

void storage_init(void)
{
    ESP_LOGI(TAG, "Initialisation du stockage externe");
    // TODO: monter la carte SD ou SPIFFS
}

void storage_export(void)
{
    ESP_LOGI(TAG, "Export des données");
    // TODO: exporter les données en CSV/JSON
}
