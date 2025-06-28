#include "lighting.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>

static const char *TAG = "lighting";


#define LEDC_TIMER   LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0

static lighting_state_t current_state = {0};
static lighting_rest_hook_t rest_hook = NULL;
static lighting_mqtt_hook_t mqtt_hook = NULL;

void lighting_init(void)
{
    ESP_LOGI(TAG, "Initialisation de l'eclairage");

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch = {
        .gpio_num = CONFIG_LIGHTING_PWM_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&ch);
}

void lighting_set_level(int level)
{
    if (level < 0)
        level = 0;
    if (level > 100)
        level = 100;

    uint32_t duty = (level * 1023) / 100;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);

    current_state.level = level;

    if (rest_hook)
        rest_hook(&current_state);
    if (mqtt_hook)
        mqtt_hook(&current_state);
}

int lighting_get_level(void)
{
    return current_state.level;
}

void lighting_set_rest_hook(lighting_rest_hook_t hook)
{
    rest_hook = hook;
}

void lighting_set_mqtt_hook(lighting_mqtt_hook_t hook)
{
    mqtt_hook = hook;
}

esp_err_t lighting_rest_post(const char *url, const lighting_state_t *state)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"level\":%d}", state->level);

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

esp_err_t lighting_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                const lighting_state_t *state)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"level\":%d}", state->level);
    int msg_id = esp_mqtt_client_publish(client, topic, json, 0, 1, 0);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}

