#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "lvgl.h"
#include "ui.h"
#include "db.h"
#include "auth.h"
#include "legal.h"
#include "legal_numbers.h"
#include "storage.h"
#include "elevages.h"
#include "animals.h"
#include "terrariums.h"
#include "stocks.h"
#include "transactions.h"
#include "scheduler.h"

/**
 * \brief Point d'entrée de l'application.
 */
void app_main(void)
{
    // Initialisation des services de base
    nvs_flash_init();

    // Initialisation de LVGL et de l'interface
    lv_init();
    ui_init(UI_LANG_EN, UI_THEME_LIGHT);
    ui_set_elevage(0);

    // Initialisation de la base de données
    db_init();

    // Initialisation de l'authentification
    auth_init();

    // Initialisation des modules metier
    legal_numbers_init();
    elevages_init();
    animals_init();
    terrariums_init(0);
    stocks_init();
    transactions_init();
    scheduler_init();

    // Boucle principale
    while (1)
    {
        // Exécution des tâches périodiques LVGL
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
