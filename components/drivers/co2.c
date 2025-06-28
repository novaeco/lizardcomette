#include "co2.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>

static const char *TAG = "co2";

#ifndef CONFIG_CO2_SDA_GPIO
#define CONFIG_CO2_SDA_GPIO 21
#endif
#ifndef CONFIG_CO2_SCL_GPIO
#define CONFIG_CO2_SCL_GPIO 22
#endif
#ifndef CONFIG_CO2_I2C_ADDR
#define CONFIG_CO2_I2C_ADDR 0x61
#endif
#define I2C_PORT I2C_NUM_0
#define I2C_FREQ_HZ 100000

static co2_rest_hook_t rest_hook = NULL;
static co2_mqtt_hook_t mqtt_hook = NULL;

void co2_init(void)
{
    ESP_LOGI(TAG, "Initialisation du capteur CO2");
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_CO2_SDA_GPIO,
        .scl_io_num = CONFIG_CO2_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_param_config(I2C_PORT, &conf);
    esp_err_t err = i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
    if (err == ESP_ERR_INVALID_STATE)
        err = ESP_OK;
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Erreur init I2C: %s", esp_err_to_name(err));
}

co2_data_t co2_read(void)
{
    co2_data_t data = {0};
    uint8_t buf[2] = {0};

    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (CONFIG_CO2_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(h, buf, sizeof(buf), I2C_MASTER_LAST_NACK);
    i2c_master_stop(h);
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, h, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur lecture CO2: %s", esp_err_to_name(err));
        return data;
    }

    data.ppm = (buf[0] << 8) | buf[1];
    ESP_LOGI(TAG, "CO2: %d ppm", data.ppm);

    if (rest_hook)
        rest_hook(&data);
    if (mqtt_hook)
        mqtt_hook(&data);

    return data;
}

void co2_set_rest_hook(co2_rest_hook_t hook)
{
    rest_hook = hook;
}

void co2_set_mqtt_hook(co2_mqtt_hook_t hook)
{
    mqtt_hook = hook;
}

esp_err_t co2_rest_post(const char *url, const co2_data_t *data)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"co2\":%u}", data->ppm);

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

esp_err_t co2_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                           const co2_data_t *data)
{
    char json[32];
    snprintf(json, sizeof(json), "{\"co2\":%u}", data->ppm);
    int msg_id = esp_mqtt_client_publish(client, topic, json, 0, 1, 0);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}

