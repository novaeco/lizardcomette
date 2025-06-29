#ifndef HUMIDITY_CTRL_H
#define HUMIDITY_CTRL_H

#include "esp_err.h"
#include "mqtt_client.h"

typedef struct {
    int level;
} humidity_state_t;

typedef void (*humidity_rest_hook_t)(const humidity_state_t *state);
typedef void (*humidity_mqtt_hook_t)(const humidity_state_t *state);

void humidity_ctrl_init(void);
void humidity_ctrl_set_level(int level);
int humidity_ctrl_get_level(void);
void humidity_ctrl_set_rest_hook(humidity_rest_hook_t hook);
void humidity_ctrl_set_mqtt_hook(humidity_mqtt_hook_t hook);
esp_err_t humidity_ctrl_rest_post(const char *url, const humidity_state_t *state);
esp_err_t humidity_ctrl_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                     const humidity_state_t *state);

#endif // HUMIDITY_CTRL_H
