#ifndef LIGHTING_H
#define LIGHTING_H

#include "esp_err.h"
#include "mqtt_client.h"

/**
 * \brief Etat du systeme d'eclairage.
 */
typedef struct {
    int level; /**< Intensite en pourcentage */
} lighting_state_t;

/** \brief Callback REST pour l'eclairage. */
typedef void (*lighting_rest_hook_t)(const lighting_state_t *state);

/** \brief Callback MQTT pour l'eclairage. */
typedef void (*lighting_mqtt_hook_t)(const lighting_state_t *state);

void lighting_init(void);

void lighting_set_level(int level);

int lighting_get_level(void);

void lighting_set_rest_hook(lighting_rest_hook_t hook);

void lighting_set_mqtt_hook(lighting_mqtt_hook_t hook);

esp_err_t lighting_rest_post(const char *url, const lighting_state_t *state);

esp_err_t lighting_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                const lighting_state_t *state);

#endif // LIGHTING_H
