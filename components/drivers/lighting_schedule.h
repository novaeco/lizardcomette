#ifndef LIGHTING_SCHEDULE_H
#define LIGHTING_SCHEDULE_H

#include "esp_err.h"
#include "mqtt_client.h"

typedef struct {
    int start_hour;
    int end_hour;
} lighting_schedule_t;

typedef void (*schedule_rest_hook_t)(const lighting_schedule_t *sched);
typedef void (*schedule_mqtt_hook_t)(const lighting_schedule_t *sched);

void lighting_schedule_init(void);
void lighting_schedule_set(lighting_schedule_t s);
lighting_schedule_t lighting_schedule_get(void);
void lighting_schedule_set_rest_hook(schedule_rest_hook_t hook);
void lighting_schedule_set_mqtt_hook(schedule_mqtt_hook_t hook);
esp_err_t lighting_schedule_rest_post(const char *url, const lighting_schedule_t *sched);
esp_err_t lighting_schedule_mqtt_publish(esp_mqtt_client_handle_t client, const char *topic,
                                         const lighting_schedule_t *sched);

#endif // LIGHTING_SCHEDULE_H
