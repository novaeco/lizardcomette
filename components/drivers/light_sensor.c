#include "light_sensor.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>

static const char *TAG = "light_sensor";

#define I2C_PORT I2C_NUM_0
#define I2C_FREQ_HZ 100000
#define BH1750_ADDR CONFIG_LIGHT_SENSOR_ADDR
#define BH1750_CMD_CONT_H_RES 0x10

static light_rest_hook_t rest_hook = NULL;
static light_mqtt_hook_t mqtt_hook = NULL;

void light_sensor_init(void)
{
    ESP_LOGI(TAG, "Initialisation du capteur de lumiere");
    // utilise le bus I2C partage configure dans drivers_init()
}

light_data_t light_sensor_read(void)
{
    light_data_t data = {0};
    uint8_t cmd = BH1750_CMD_CONT_H_RES;
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (BH1750_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(h, cmd, true);
    i2c_master_stop(h);
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, h, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur envoi commande: %s", esp_err_to_name(err));
        return data;
    }
    vTaskDelay(pdMS_TO_TICKS(180));
    uint8_t buf[2] = {0};
    h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (BH1750_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(h, buf, sizeof(buf), I2C_MASTER_LAST_NACK);
    i2c_master_stop(h);
    err = i2c_master_cmd_begin(I2C_PORT, h, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur lecture: %s", esp_err_to_name(err));
        return data;
    }
    uint16_t raw = (buf[0] << 8) | buf[1];
    data.lux = raw / 1.2f;
    ESP_LOGI(TAG, "Luminosite: %.1f lux", data.lux);
    if (rest_hook)
        rest_hook(&data);
    if (mqtt_hook)
        mqtt_hook(&data);
    return data;
}

void light_sensor_set_rest_hook(light_rest_hook_t hook)
{
    rest_hook = hook;
}

void light_sensor_set_mqtt_hook(light_mqtt_hook_t hook)
{
    mqtt_hook = hook;
}

esp_err_t light_sensor_rest_post(const char *url, const light_data_t *data)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"lux\":%.1f}", data->lux);
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

esp_err_t light_sensor_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                    const light_data_t *data)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"lux\":%.1f}", data->lux);
    int msg_id = esp_mqtt_client_publish(client, topic, json, 0, 1, 0);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}

