#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdint.h>

/**
 * \brief Données lues par les capteurs.
 */
typedef struct {
    float temperature_c;
    float humidity_percent;
} sensor_data_t;

/** \brief Prototype pour extension REST. */
typedef void (*drivers_rest_hook_t)(const sensor_data_t *data);

/** \brief Prototype pour extension MQTT. */
typedef void (*drivers_mqtt_hook_t)(const sensor_data_t *data);

void drivers_init(void);

sensor_data_t drivers_read(void);

void drivers_set_rest_hook(drivers_rest_hook_t hook);

void drivers_set_mqtt_hook(drivers_mqtt_hook_t hook);

#endif // DRIVERS_H
