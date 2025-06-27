#include "terrariums.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "terrariums";

static Terrarium terrariums[TERRARIUMS_MAX];
static int terrarium_count = 0;
static char logs[LOGS_MAX][64];
static int log_count = 0;

void terrariums_init(void)
{
    terrarium_count = 0;
    log_count = 0;
    memset(terrariums, 0, sizeof(terrariums));
    ESP_LOGI(TAG, "Initialisation des terrariums");
}

bool terrariums_add(const Terrarium *t)
{
    if (terrarium_count >= TERRARIUMS_MAX || !t)
        return false;
    terrariums[terrarium_count] = *t;
    terrarium_count++;
    ESP_LOGI(TAG, "Ajout du terrarium %d", t->id);
    return true;
}

static int find_index(int id)
{
    for (int i = 0; i < terrarium_count; ++i) {
        if (terrariums[i].id == id)
            return i;
    }
    return -1;
}

const Terrarium *terrariums_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &terrariums[idx];
    return NULL;
}

bool terrariums_update(int id, const Terrarium *t)
{
    int idx = find_index(id);
    if (idx < 0 || !t)
        return false;
    terrariums[idx] = *t;
    ESP_LOGI(TAG, "Mise a jour du terrarium %d", id);
    return true;
}

bool terrariums_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    for (int i = idx; i < terrarium_count - 1; ++i) {
        terrariums[i] = terrariums[i + 1];
    }
    terrarium_count--;
    ESP_LOGI(TAG, "Suppression du terrarium %d", id);
    return true;
}

void terrariums_log_transaction(const char *msg)
{
    if (log_count >= LOGS_MAX || !msg)
        return;
    strncpy(logs[log_count], msg, sizeof(logs[0]) - 1);
    logs[log_count][sizeof(logs[0]) - 1] = '\0';
    ESP_LOGI(TAG, "Log: %s", logs[log_count]);
    log_count++;
}
