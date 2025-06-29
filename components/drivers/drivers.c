#include "drivers.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "mqtt_client.h"
#include <string.h>
#include "humidity_ctrl.h"
#include "lighting_schedule.h"
#include "lighting.h"
#include "co2.h"
#include "light_sensor.h"

static const char *TAG = "drivers";

#define I2C_PORT           I2C_NUM_0
#define I2C_FREQ_HZ        100000

#define SHT3X_ADDR         0x44

static drivers_rest_hook_t rest_hook = NULL;
static drivers_mqtt_hook_t mqtt_hook = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;

static void ensure_mqtt_client(void)
{
    if (!strlen(CONFIG_DRIVERS_MQTT_URI))
        return;
    if (!mqtt_client) {
        esp_mqtt_client_config_t cfg = { .uri = CONFIG_DRIVERS_MQTT_URI };
        mqtt_client = esp_mqtt_client_init(&cfg);
        if (mqtt_client)
            esp_mqtt_client_start(mqtt_client);
    }
}

static void auto_rest(const sensor_data_t *data)
{
    if (strlen(CONFIG_DRIVERS_REST_URL))
        drivers_rest_post(CONFIG_DRIVERS_REST_URL, data);
}

static void auto_mqtt(const sensor_data_t *data)
{
    ensure_mqtt_client();
    if (mqtt_client)
        drivers_mqtt_publish(mqtt_client, CONFIG_DRIVERS_MQTT_TOPIC, data);
}

static void auto_lighting_rest(const lighting_state_t *state)
{
    if (strlen(CONFIG_LIGHTING_REST_URL))
        lighting_rest_post(CONFIG_LIGHTING_REST_URL, state);
}

static void auto_lighting_mqtt(const lighting_state_t *state)
{
    ensure_mqtt_client();
    if (mqtt_client)
        lighting_mqtt_publish(mqtt_client, CONFIG_LIGHTING_MQTT_TOPIC, state);
}

static void auto_humidity_rest(const humidity_state_t *state)
{
    if (strlen(CONFIG_HUMIDITY_REST_URL))
        humidity_ctrl_rest_post(CONFIG_HUMIDITY_REST_URL, state);
}

static void auto_humidity_mqtt(const humidity_state_t *state)
{
    ensure_mqtt_client();
    if (mqtt_client)
        humidity_ctrl_mqtt_publish(mqtt_client, CONFIG_HUMIDITY_MQTT_TOPIC, state);
}

static void auto_co2_rest(const co2_data_t *data)
{
    if (strlen(CONFIG_CO2_REST_URL))
        co2_rest_post(CONFIG_CO2_REST_URL, data);
}

static void auto_co2_mqtt(const co2_data_t *data)
{
    ensure_mqtt_client();
    if (mqtt_client)
        co2_mqtt_publish(mqtt_client, CONFIG_CO2_MQTT_TOPIC, data);
}

static void auto_light_sensor_rest(const light_data_t *data)
{
    if (strlen(CONFIG_LIGHT_SENSOR_REST_URL))
        light_sensor_rest_post(CONFIG_LIGHT_SENSOR_REST_URL, data);
}

static void auto_light_sensor_mqtt(const light_data_t *data)
{
    ensure_mqtt_client();
    if (mqtt_client)
        light_sensor_mqtt_publish(mqtt_client, CONFIG_LIGHT_SENSOR_MQTT_TOPIC, data);
}

static void auto_schedule_rest(const lighting_schedule_t *sched)
{
    if (strlen(CONFIG_LIGHTING_SCHEDULE_REST_URL))
        lighting_schedule_rest_post(CONFIG_LIGHTING_SCHEDULE_REST_URL, sched);
}

static void auto_schedule_mqtt(const lighting_schedule_t *sched)
{
    ensure_mqtt_client();
    if (mqtt_client)
        lighting_schedule_mqtt_publish(mqtt_client, CONFIG_LIGHTING_SCHEDULE_MQTT_TOPIC, sched);
}

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

    light_sensor_init();
    co2_init();
    lighting_init();
    humidity_ctrl_init();
    lighting_schedule_init();

    light_sensor_set_rest_hook(auto_light_sensor_rest);
    light_sensor_set_mqtt_hook(auto_light_sensor_mqtt);
    co2_set_rest_hook(auto_co2_rest);
    co2_set_mqtt_hook(auto_co2_mqtt);
    lighting_set_rest_hook(auto_lighting_rest);
    lighting_set_mqtt_hook(auto_lighting_mqtt);
    humidity_ctrl_set_rest_hook(auto_humidity_rest);
    humidity_ctrl_set_mqtt_hook(auto_humidity_mqtt);
    lighting_schedule_set_rest_hook(auto_schedule_rest);
    lighting_schedule_set_mqtt_hook(auto_schedule_mqtt);

    drivers_set_rest_hook(auto_rest);
    drivers_set_mqtt_hook(auto_mqtt);
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
