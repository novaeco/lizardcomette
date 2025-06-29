#include "drivers.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "mqtt_client.h"
#include <string.h>

static const char *TAG = "drivers";

#define I2C_PORT           I2C_NUM_0
#define I2C_FREQ_HZ        100000

#define SHT3X_ADDR         0x44

static drivers_rest_hook_t rest_hook = NULL;
static drivers_mqtt_hook_t mqtt_hook = NULL;

void drivers_init(void)
{
    ESP_LOGI(TAG, "Initialisation des capteurs");

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_I2C_SDA_GPIO,
        .scl_io_num = CONFIG_I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
}

sensor_data_t drivers_read(void)
{
    sensor_data_t data = {0};

    uint8_t cmd[2] = {0x24, 0x00};
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (SHT3X_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(h, cmd, sizeof(cmd), true);
    i2c_master_stop(h);
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, h, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur I2C write: %s", esp_err_to_name(err));
        return data;
    }

    vTaskDelay(pdMS_TO_TICKS(15));

    uint8_t buf[6] = {0};
    h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (SHT3X_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(h, buf, sizeof(buf), I2C_MASTER_LAST_NACK);
    i2c_master_stop(h);
    err = i2c_master_cmd_begin(I2C_PORT, h, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur I2C read: %s", esp_err_to_name(err));
        return data;
    }

    uint16_t raw_t = (buf[0] << 8) | buf[1];
    uint16_t raw_h = (buf[3] << 8) | buf[4];
    data.temperature_c = -45 + 175 * (raw_t / 65535.0f);
    data.humidity_percent = 100 * (raw_h / 65535.0f);

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

esp_err_t drivers_rest_post(const char *url, const sensor_data_t *data)
{
    char json[64];
    snprintf(json, sizeof(json), "{\"temperature\":%.2f,\"humidity\":%.2f}",
             data->temperature_c, data->humidity_percent);

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

esp_err_t drivers_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                               const sensor_data_t *data)
{
    char json[64];
    snprintf(json, sizeof(json), "{\"temperature\":%.2f,\"humidity\":%.2f}",
             data->temperature_c, data->humidity_percent);
    int msg_id = esp_mqtt_client_publish(client, topic, json, 0, 1, 0);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}
