#include "ui.h"
#include "esp_log.h"
#include "animals.h"
#include "terrariums.h"
#include "stocks.h"
#include "transactions.h"
#include "legal.h"
#include "storage.h"

static const char *TAG = "ui";

static ui_language_t current_lang = UI_LANG_EN;
static ui_theme_t current_theme = UI_THEME_LIGHT;
static int current_elevage_id = 0;

static const char *translations[UI_LANG_COUNT][TXT_COUNT] = {
    [UI_LANG_EN] = {
        [TXT_ANIMALS] = "Animals",
        [TXT_TERRARIUMS] = "Terrariums",
        [TXT_STOCKS] = "Stock",
        [TXT_TRANSACTIONS] = "Transactions",
        [TXT_SETTINGS] = "Settings",
        [TXT_EXPORT] = "Export",
        [TXT_IMPORT] = "Import",
        [TXT_LEGAL_OK] = "OK",
        [TXT_LEGAL_EXPIRED] = "Expired",
        [TXT_LANGUAGE] = "Language",
        [TXT_THEME] = "Theme",
    },
    [UI_LANG_FR] = {
        [TXT_ANIMALS] = "Animaux",
        [TXT_TERRARIUMS] = "Terrariums",
        [TXT_STOCKS] = "Stocks",
        [TXT_TRANSACTIONS] = "Transactions",
        [TXT_SETTINGS] = "Param\xC3\xA8tres",
        [TXT_EXPORT] = "Exporter",
        [TXT_IMPORT] = "Importer",
        [TXT_LEGAL_OK] = "OK",
        [TXT_LEGAL_EXPIRED] = "Expire",
        [TXT_LANGUAGE] = "Langue",
        [TXT_THEME] = "Th\xC3\xA8me",
    }
};

static lv_obj_t *tabview;
static lv_obj_t *notif_label;
static lv_obj_t *tab_animals;
static lv_obj_t *tab_terrariums;

static void animals_tab_create(lv_obj_t *tab)
{
    lv_obj_t *list = lv_list_create(tab);
    lv_obj_set_size(list, 380, 420);
    for (int i = 0; i < animals_count_for_elevage(current_elevage_id); ++i) {
        const Reptile *r = animals_get_by_index_for_elevage(i, current_elevage_id);
        if (!r) continue;
        const char *status = (legal_is_cerfa_valid(r) && legal_is_cites_valid(r)) ?
                                ui_get_text(TXT_LEGAL_OK) : ui_get_text(TXT_LEGAL_EXPIRED);
        lv_obj_t *btn = lv_list_add_btn(list, NULL, r->name);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, status);
        lv_obj_align(lbl, LV_ALIGN_RIGHT_MID, -10, 0);
    }
}

static void terrariums_tab_create(lv_obj_t *tab)
{
    lv_obj_t *list = lv_list_create(tab);
    lv_obj_set_size(list, 380, 420);
    for (int i = 0; i < terrariums_count_for_elevage(current_elevage_id); ++i) {
        const Terrarium *t = terrariums_get_by_index_for_elevage(i, current_elevage_id);
        if (!t) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%s (%d)", t->name, t->capacity);
        lv_list_add_btn(list, NULL, buf);
    }
}

static void stocks_tab_create(lv_obj_t *tab)
{
    lv_obj_t *list = lv_list_create(tab);
    lv_obj_set_size(list, 380, 420);
    for (int i = 0; i < stocks_count(); ++i) {
        const StockItem *s = stocks_get_by_index(i);
        if (!s) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%s (%d)", s->name, s->quantity);
        lv_list_add_btn(list, NULL, buf);
    }
}

static void transactions_tab_create(lv_obj_t *tab)
{
    lv_obj_t *list = lv_list_create(tab);
    lv_obj_set_size(list, 380, 420);
    for (int i = 0; i < transactions_count(); ++i) {
        const Transaction *tr = transactions_get_by_index(i);
        if (!tr) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%d %s %d", tr->stock_id,
                 tr->type == TRANSACTION_PURCHASE ? "+" : "-",
                 tr->quantity);
        lv_list_add_btn(list, NULL, buf);
    }
}

void ui_set_elevage(int elevage_id)
{
    current_elevage_id = elevage_id;
    if (tab_animals && tab_terrariums) {
        lv_obj_clean(tab_animals);
        lv_obj_clean(tab_terrariums);
        animals_tab_create(tab_animals);
        terrariums_tab_create(tab_terrariums);
    }
}

int ui_get_elevage(void)
{
    return current_elevage_id;
}

static void export_event(lv_event_t *e)
{
    storage_export_csv("/sdcard/export.csv");
    ui_notify("Export OK");
}

static void import_event(lv_event_t *e)
{
    storage_restore();
    ui_notify("Import OK");
}

static void lang_event(lv_event_t *e)
{
    uint32_t sel = lv_dropdown_get_selected(lv_event_get_target(e));
    ui_set_language(sel == 0 ? UI_LANG_EN : UI_LANG_FR);
}

static void theme_event(lv_event_t *e)
{
    uint32_t sel = lv_dropdown_get_selected(lv_event_get_target(e));
    ui_set_theme(sel == 0 ? UI_THEME_LIGHT : UI_THEME_DARK);
}

static void settings_tab_create(lv_obj_t *tab)
{
    lv_obj_t *dd_lang = lv_dropdown_create(tab);
    lv_dropdown_set_options(dd_lang, "English\nFrancais");
    lv_obj_add_event_cb(dd_lang, lang_event, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *dd_theme = lv_dropdown_create(tab);
    lv_obj_align(dd_theme, LV_ALIGN_TOP_MID, 0, 40);
    lv_dropdown_set_options(dd_theme, "Light\nDark");
    lv_obj_add_event_cb(dd_theme, theme_event, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *btn_export = lv_btn_create(tab);
    lv_obj_align(btn_export, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_export, export_event, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl1 = lv_label_create(btn_export);
    lv_label_set_text(lbl1, ui_get_text(TXT_EXPORT));

    lv_obj_t *btn_import = lv_btn_create(tab);
    lv_obj_align(btn_import, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_import, import_event, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl2 = lv_label_create(btn_import);
    lv_label_set_text(lbl2, ui_get_text(TXT_IMPORT));
}

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
        lv_tabview_set_tab_name(tabview, 2, ui_get_text(TXT_STOCKS));
        lv_tabview_set_tab_name(tabview, 3, ui_get_text(TXT_TRANSACTIONS));
        lv_tabview_set_tab_name(tabview, 4, ui_get_text(TXT_SETTINGS));
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

    tab_animals = lv_tabview_add_tab(tabview, ui_get_text(TXT_ANIMALS));
    tab_terrariums = lv_tabview_add_tab(tabview, ui_get_text(TXT_TERRARIUMS));
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, ui_get_text(TXT_STOCKS));
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, ui_get_text(TXT_TRANSACTIONS));
    lv_obj_t *tab5 = lv_tabview_add_tab(tabview, ui_get_text(TXT_SETTINGS));

    animals_tab_create(tab_animals);
    terrariums_tab_create(tab_terrariums);
    stocks_tab_create(tab3);
    transactions_tab_create(tab4);
    settings_tab_create(tab5);

    notif_label = lv_label_create(lv_scr_act());
    lv_obj_align(notif_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(notif_label, "");
}

void ui_notify(const char *msg)
{
    if (!notif_label || !msg)
        return;
    lv_label_set_text(notif_label, msg);
}
