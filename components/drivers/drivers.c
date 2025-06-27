#include "drivers.h"
#include "esp_log.h"

static const char *TAG = "drivers";

static drivers_rest_hook_t rest_hook = NULL;
static drivers_mqtt_hook_t mqtt_hook = NULL;

void drivers_init(void)
{
    ESP_LOGI(TAG, "Initialisation des capteurs");
    // Ici on pourrait initialiser de vrais capteurs I2C/SPI
}

sensor_data_t drivers_read(void)
{
    sensor_data_t data = {0};
    // Valeurs simulées pour l'exemple
    data.temperature_c = 25.0f;
    data.humidity_percent = 50.0f;

    ESP_LOGI(TAG, "Temp: %.1fC Hum: %.1f%%", data.temperature_c, data.humidity_percent);

    if (rest_hook)
        rest_hook(&data);
    if (mqtt_hook)
        mqtt_hook(&data);

    return data;
}

void drivers_set_rest_hook(drivers_rest_hook_t hook)
{
    rest_hook = hook;
}

void drivers_set_mqtt_hook(drivers_mqtt_hook_t hook)
{
    mqtt_hook = hook;
}
