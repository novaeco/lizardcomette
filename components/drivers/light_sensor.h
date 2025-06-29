#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>
#include "esp_err.h"
#include "mqtt_client.h"

/**
 * \brief Mesure de luminosité en lux.
 */
typedef struct {
    float lux;
} light_data_t;

/** \brief Callback REST pour le capteur de lumière. */
typedef void (*light_rest_hook_t)(const light_data_t *data);

/** \brief Callback MQTT pour le capteur de lumière. */
typedef void (*light_mqtt_hook_t)(const light_data_t *data);

void light_sensor_init(void);

light_data_t light_sensor_read(void);

void light_sensor_set_rest_hook(light_rest_hook_t hook);

void light_sensor_set_mqtt_hook(light_mqtt_hook_t hook);

esp_err_t light_sensor_rest_post(const char *url, const light_data_t *data);

esp_err_t light_sensor_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                    const light_data_t *data);

#endif // LIGHT_SENSOR_H
