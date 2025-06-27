#include "scheduler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ui.h"

#define SCHEDULER_INTERVAL_MS 60000

static const char *TAG = "scheduler";

static void notify(const char *msg)
{
    ESP_LOGI(TAG, "%s", msg);
    ui_notify(msg);
}

static void check_regulatory_deadlines(void)
{
    notify("Verification des echeances reglementaires");
}

static void check_stock_levels(void)
{
    notify("Verification des niveaux de stock");
}

static void check_compliance(void)
{
    notify("Verification de la conformite");
}

static void scheduler_task(void *arg)
{
    (void)arg;
    while (1) {
        check_regulatory_deadlines();
        check_stock_levels();
        check_compliance();
        vTaskDelay(pdMS_TO_TICKS(SCHEDULER_INTERVAL_MS));
    }
}

void scheduler_init(void)
{
    ESP_LOGI(TAG, "Initialisation du planificateur");
    xTaskCreate(scheduler_task, "scheduler", 4096, NULL, 5, NULL);
}
