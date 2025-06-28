#include "scheduler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ui.h"
#include "stocks.h"
#include "legal.h"
#include "animals.h"
#include "health.h"
#include "breeding.h"
#include <time.h>

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
    legal_check_documents();
}

static void check_stock_levels(void)
{
    for (int i = 0; i < stocks_count(); ++i) {
        const StockItem *it = stocks_get_by_index(i);
        if (it && it->quantity <= it->min_quantity) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Stock bas: %s", it->name);
            notify(msg);
        }
    }
}

static void check_compliance(void)
{
    notify("Verification de la conformite");
    for (int i = 0; i < animals_count(); ++i) {
        const Reptile *r = animals_get_by_index(i);
        if (!r)
            continue;
        if (!legal_is_cdc_valid(r))
            notify("CDC invalide");
        if (!legal_is_aoe_valid(r))
            notify("AOE invalide");
        if (!legal_quota_remaining(r))
            notify("Quota atteint");
    }
}

static void check_health_alerts(void)
{
    time_t now = time(NULL);
    for (int i = 0; i < health_count(); ++i) {
        const HealthRecord *r = health_get_by_index(i);
        if (r && r->date <= now + 86400 && r->date >= now) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Controle sante animal %d", r->animal_id);
            notify(msg);
        }
    }
}

static void check_breeding_alerts(void)
{
    time_t now = time(NULL);
    for (int i = 0; i < breeding_count(); ++i) {
        const BreedingEvent *e = breeding_get_by_index(i);
        if (e && e->date <= now + 86400 && e->date >= now) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Evenement reproduction animal %d", e->animal_id);
            notify(msg);
        }
    }
}

static void scheduler_task(void *arg)
{
    (void)arg;
    while (1) {
        check_regulatory_deadlines();
        check_stock_levels();
        check_compliance();
        check_health_alerts();
        check_breeding_alerts();
        vTaskDelay(pdMS_TO_TICKS(SCHEDULER_INTERVAL_MS));
    }
}

void scheduler_init(void)
{
    ESP_LOGI(TAG, "Initialisation du planificateur");
    xTaskCreate(scheduler_task, "scheduler", 4096, NULL, 5, NULL);
}

void scheduler_check_stock_levels(void)
{
    check_stock_levels();
}

void scheduler_check_regulatory_deadlines(void)
{
    check_regulatory_deadlines();
}

void scheduler_check_compliance(void)
{
    check_compliance();
}
