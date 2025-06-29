#include "lighting_schedule.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>

static const char *TAG = "lighting_sched";

static lighting_schedule_t current_schedule = {8, 20};
static schedule_rest_hook_t rest_hook = NULL;
static schedule_mqtt_hook_t mqtt_hook = NULL;

void lighting_schedule_init(void)
{
    ESP_LOGI(TAG, "Initialisation du programme d'eclairage");
}

void lighting_schedule_set(lighting_schedule_t s)
{
    current_schedule = s;
    if (rest_hook)
        rest_hook(&current_schedule);
    if (mqtt_hook)
        mqtt_hook(&current_schedule);
}

lighting_schedule_t lighting_schedule_get(void)
{
    return current_schedule;
}

void lighting_schedule_set_rest_hook(schedule_rest_hook_t hook)
{
    rest_hook = hook;
}

void lighting_schedule_set_mqtt_hook(schedule_mqtt_hook_t hook)
{
    mqtt_hook = hook;
}

esp_err_t lighting_schedule_rest_post(const char *url, const lighting_schedule_t *sched)
{
    char json[64];
    snprintf(json, sizeof(json), "{\"start\":%d,\"end\":%d}", sched->start_hour, sched->end_hour);

    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client)
        return ESP_FAIL;

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json, strlen(json));
    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    return err;
}

esp_err_t lighting_schedule_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                         const lighting_schedule_t *sched)
{
    char json[64];
    snprintf(json, sizeof(json), "{\"start\":%d,\"end\":%d}", sched->start_hour, sched->end_hour);
    int msg_id = esp_mqtt_client_publish(client, topic, json, 0, 1, 0);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}

