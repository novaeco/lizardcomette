#include "ui.h"
#include "esp_log.h"

static const char *TAG = "ui";

static ui_language_t current_lang = UI_LANG_EN;
static ui_theme_t current_theme = UI_THEME_LIGHT;

static const char *translations[UI_LANG_COUNT][TXT_COUNT] = {
    [UI_LANG_EN] = {
        [TXT_ANIMALS] = "Animals",
        [TXT_TERRARIUMS] = "Terrariums",
        [TXT_SETTINGS] = "Settings",
    },
    [UI_LANG_FR] = {
        [TXT_ANIMALS] = "Animaux",
        [TXT_TERRARIUMS] = "Terrariums",
        [TXT_SETTINGS] = "Param\xC3\xA8tres",
    }
};

static lv_obj_t *tabview;

const char *ui_get_text(ui_text_id_t id)
{
    if (id >= TXT_COUNT)
        return "";
    return translations[current_lang][id];
}

void ui_set_language(ui_language_t lang)
{
    if (lang >= UI_LANG_COUNT)
        return;
    current_lang = lang;
    if (tabview) {
        lv_tabview_set_tab_name(tabview, 0, ui_get_text(TXT_ANIMALS));
        lv_tabview_set_tab_name(tabview, 1, ui_get_text(TXT_TERRARIUMS));
        lv_tabview_set_tab_name(tabview, 2, ui_get_text(TXT_SETTINGS));
    }
}

static void apply_theme(void)
{
    lv_theme_t *th = lv_theme_default_init(NULL,
                                           lv_palette_main(LV_PALETTE_BLUE),
                                           lv_palette_main(LV_PALETTE_RED),
                                           current_theme == UI_THEME_DARK,
                                           LV_FONT_DEFAULT);
    lv_disp_set_theme(NULL, th);
}

void ui_set_theme(ui_theme_t theme)
{
    if (theme >= UI_THEME_COUNT)
        return;
    current_theme = theme;
    apply_theme();
}

void ui_init(ui_language_t lang, ui_theme_t theme)
{
    current_lang = lang;
    current_theme = theme;

    ESP_LOGI(TAG, "Initialisation de l'interface LVGL");

    apply_theme();

    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
    lv_obj_set_size(tabview, 800, 480);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, ui_get_text(TXT_ANIMALS));
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, ui_get_text(TXT_TERRARIUMS));
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, ui_get_text(TXT_SETTINGS));

    lv_label_create(tab1);
    lv_label_set_text(lv_obj_get_child(tab1, 0), ui_get_text(TXT_ANIMALS));

    lv_label_create(tab2);
    lv_label_set_text(lv_obj_get_child(tab2, 0), ui_get_text(TXT_TERRARIUMS));

    lv_label_create(tab3);
    lv_label_set_text(lv_obj_get_child(tab3, 0), ui_get_text(TXT_SETTINGS));
}
