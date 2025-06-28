#ifndef CO2_H
#define CO2_H

#include <stdint.h>
#include "esp_err.h"
#include "mqtt_client.h"

/**
 * \brief Mesure du CO2 en ppm.
 */
typedef struct {
    uint16_t ppm;
} co2_data_t;

/** \brief Callback REST pour le CO2. */
typedef void (*co2_rest_hook_t)(const co2_data_t *data);

/** \brief Callback MQTT pour le CO2. */
typedef void (*co2_mqtt_hook_t)(const co2_data_t *data);

void co2_init(void);

co2_data_t co2_read(void);

void co2_set_rest_hook(co2_rest_hook_t hook);

void co2_set_mqtt_hook(co2_mqtt_hook_t hook);

esp_err_t co2_rest_post(const char *url, const co2_data_t *data);

esp_err_t co2_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                           const co2_data_t *data);

#endif // CO2_H
