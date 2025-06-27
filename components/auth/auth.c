#include "auth.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "auth";
static const char *stored_hash = "changeme"; // TODO: stocker un hash sécurisé

void auth_init(void)
{
    ESP_LOGI(TAG, "Initialisation du module d'authentification");
}

bool auth_check(const char *password)
{
    // TODO: implémenter un hachage et une vérification sécurisée
    return strcmp(password, stored_hash) == 0;
}
