#include "ui.h"
#include "esp_log.h"
#include "animals.h"
#include "terrariums.h"
#include "stocks.h"
#include "transactions.h"
#include "legal.h"
#include "health.h"
#include "breeding.h"
#include "storage.h"
#include "auth.h"
#include "elevages.h"
#include <string.h>
#include <stdlib.h>

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
        [TXT_ELEVAGES] = "Farms",
        [TXT_SETTINGS] = "Settings",
        [TXT_EXPORT] = "Export",
        [TXT_IMPORT] = "Import",
        [TXT_LEGAL_OK] = "OK",
        [TXT_LEGAL_EXPIRED] = "Expired",
        [TXT_LANGUAGE] = "Language",
        [TXT_THEME] = "Theme",
        [TXT_LOGIN] = "Login",
        [TXT_USERNAME] = "Username",
        [TXT_PASSWORD] = "Password",
        [TXT_ADD] = "Add",
        [TXT_SAVE] = "Save",
        [TXT_DELETE] = "Delete",
        [TXT_OK] = "OK",
        [TXT_CDC_NUMBER] = "CDC number",
        [TXT_AOE_NUMBER] = "AOE number",
        [TXT_IFAP_ID] = "I-FAP ID",
        [TXT_QUOTA_LIMIT] = "Quota limit",
        [TXT_QUOTA_USED] = "Quota used",
        [TXT_CERFA_EXPIRY] = "Cerfa expiry",
        [TXT_CITES_EXPIRY] = "CITES expiry",
    },
    [UI_LANG_FR] = {
        [TXT_ANIMALS] = "Animaux",
        [TXT_TERRARIUMS] = "Terrariums",
        [TXT_STOCKS] = "Stocks",
        [TXT_TRANSACTIONS] = "Transactions",
        [TXT_ELEVAGES] = "Elevages",
        [TXT_SETTINGS] = "Param\xC3\xA8tres",
        [TXT_EXPORT] = "Exporter",
        [TXT_IMPORT] = "Importer",
        [TXT_LEGAL_OK] = "OK",
        [TXT_LEGAL_EXPIRED] = "Expire",
        [TXT_LANGUAGE] = "Langue",
        [TXT_THEME] = "Th\xC3\xA8me",
        [TXT_LOGIN] = "Connexion",
        [TXT_USERNAME] = "Utilisateur",
        [TXT_PASSWORD] = "Mot de passe",
        [TXT_ADD] = "Ajouter",
        [TXT_SAVE] = "Sauvegarder",
        [TXT_DELETE] = "Supprimer",
        [TXT_OK] = "OK",
        [TXT_CDC_NUMBER] = "N° CDC",
        [TXT_AOE_NUMBER] = "N° AOE",
        [TXT_IFAP_ID] = "Identifiant I-FAP",
        [TXT_QUOTA_LIMIT] = "Quota limite",
        [TXT_QUOTA_USED] = "Quota utilisé",
        [TXT_CERFA_EXPIRY] = "Validité Cerfa",
        [TXT_CITES_EXPIRY] = "Validité CITES",
    }
};

static lv_obj_t *tabview;
static lv_obj_t *notif_label;
static lv_obj_t *tab_animals;
static lv_obj_t *tab_terrariums;
static lv_obj_t *tab_stocks;
static lv_obj_t *tab_transactions;
static lv_obj_t *tab_elevages;
static lv_obj_t *list_animals;
static lv_obj_t *list_terrariums;
static lv_obj_t *list_stocks;
static lv_obj_t *list_transactions;
static lv_obj_t *list_elevages;

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_name;
    lv_obj_t *ta_species;
    lv_obj_t *ta_sex;
    lv_obj_t *ta_birth;
    lv_obj_t *ta_cdc;
    lv_obj_t *ta_aoe;
    lv_obj_t *ta_ifap;
    lv_obj_t *ta_quota_limit;
    lv_obj_t *ta_quota_used;
    lv_obj_t *ta_cerfa;
    lv_obj_t *ta_cites;
    bool is_new;
    int orig_id;
} AnimalFormCtx;

static AnimalFormCtx animal_form;

static void close_win_event(lv_event_t *e)
{
    lv_obj_del(lv_obj_get_parent(lv_event_get_current_target(e)));
}

static void open_health_list(int animal_id)
{
    lv_obj_t *win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(win, 300, 200);
    lv_obj_center(win);

    lv_obj_t *list = lv_list_create(win);
    lv_obj_set_size(list, 280, 160);
    for (int i = 0; i < health_count_for_animal(animal_id); ++i) {
        const HealthRecord *r = health_get_by_index_for_animal(i, animal_id);
        if (!r) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%d %s", r->date, r->description);
        lv_list_add_btn(list, NULL, buf);
    }

    lv_obj_t *btn = lv_btn_create(win);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn, close_win_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn), "Close");
}

static void open_breeding_list(int animal_id)
{
    lv_obj_t *win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(win, 300, 200);
    lv_obj_center(win);

    lv_obj_t *list = lv_list_create(win);
    lv_obj_set_size(list, 280, 160);
    for (int i = 0; i < breeding_count_for_animal(animal_id); ++i) {
        const BreedingEvent *ev = breeding_get_by_index_for_animal(i, animal_id);
        if (!ev) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%d %s", ev->date, ev->description);
        lv_list_add_btn(list, NULL, buf);
    }

    lv_obj_t *btn = lv_btn_create(win);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn, close_win_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn), "Close");
}

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_name;
    lv_obj_t *ta_capacity;
    bool is_new;
    int orig_id;
} TerrariumFormCtx;

static TerrariumFormCtx terrarium_form;

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_name;
    lv_obj_t *ta_qty;
    lv_obj_t *ta_min;
    bool is_new;
    int orig_id;
} StockFormCtx;

static StockFormCtx stock_form;

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_stock_id;
    lv_obj_t *ta_qty;
    lv_obj_t *dd_type;
    bool is_new;
    int orig_id;
} TransactionFormCtx;

static TransactionFormCtx transaction_form;

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_name;
    lv_obj_t *ta_desc;
    bool is_new;
    int orig_id;
} ElevageFormCtx;

static ElevageFormCtx elevage_form;
static lv_obj_t *login_win;
static lv_obj_t *ta_user;
static lv_obj_t *ta_pass;
static lv_obj_t *btn_login;
static lv_obj_t *lbl_login;
static char logged_user[32];
static user_role_t logged_role;
static bool logged_in = false;
static lv_obj_t *elevage_win;
static lv_obj_t *dd_elevage;

static void open_elevage_select(void);

static void login_event(lv_event_t *e)
{
    const char *user = lv_textarea_get_text(ta_user);
    const char *pass = lv_textarea_get_text(ta_pass);
    if (auth_check(user, pass)) {
        strncpy(logged_user, user, sizeof(logged_user) - 1);
        logged_user[sizeof(logged_user) - 1] = '\0';
        logged_role = auth_get_role(user);
        logged_in = true;
        lv_obj_del(login_win);
        open_elevage_select();
    } else {
        ui_notify("Login failed");
    }
}

static void create_login(void)
{
    login_win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(login_win, 300, 180);
    lv_obj_center(login_win);

    ta_user = lv_textarea_create(login_win);
    lv_obj_set_width(ta_user, 280);
    lv_textarea_set_placeholder_text(ta_user, ui_get_text(TXT_USERNAME));
    lv_obj_align(ta_user, LV_ALIGN_TOP_MID, 0, 10);

    ta_pass = lv_textarea_create(login_win);
    lv_obj_set_width(ta_pass, 280);
    lv_textarea_set_placeholder_text(ta_pass, ui_get_text(TXT_PASSWORD));
    lv_textarea_set_password_mode(ta_pass, true);
    lv_obj_align(ta_pass, LV_ALIGN_TOP_MID, 0, 60);

    btn_login = lv_btn_create(login_win);
    lv_obj_align(btn_login, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn_login, login_event, LV_EVENT_CLICKED, NULL);
    lbl_login = lv_label_create(btn_login);
    lv_label_set_text(lbl_login, ui_get_text(TXT_LOGIN));
}

static void elevage_ok_event(lv_event_t *e)
{
    (void)e;
    uint32_t sel = lv_dropdown_get_selected(dd_elevage);
    int id = auth_get_elevage_by_index(logged_user, sel);
    ui_set_elevage(id);
    lv_obj_del(elevage_win);
    build_tabs();
    notif_label = lv_label_create(lv_scr_act());
    lv_obj_align(notif_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(notif_label, "");
}

static void open_elevage_select(void)
{
    int count = auth_get_elevage_count(logged_user);
    if (count <= 0) {
        build_tabs();
        notif_label = lv_label_create(lv_scr_act());
        lv_obj_align(notif_label, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_label_set_text(notif_label, "");
        return;
    }

    if (count == 1) {
        int id = auth_get_elevage_by_index(logged_user, 0);
        ui_set_elevage(id);
        build_tabs();
        notif_label = lv_label_create(lv_scr_act());
        lv_obj_align(notif_label, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_label_set_text(notif_label, "");
        return;
    }

    elevage_win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(elevage_win, 300, 150);
    lv_obj_center(elevage_win);

    dd_elevage = lv_dropdown_create(elevage_win);
    lv_obj_set_width(dd_elevage, 280);
    char opts[256] = "";
    for (int i = 0; i < count; ++i) {
        int id = auth_get_elevage_by_index(logged_user, i);
        const Elevage *ev = elevages_get(id);
        const char *name = ev ? ev->name : "";
        strcat(opts, name);
        if (i < count - 1)
            strcat(opts, "\n");
    }
    lv_dropdown_set_options(dd_elevage, opts);

    lv_obj_t *btn = lv_btn_create(elevage_win);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn, elevage_ok_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn), ui_get_text(TXT_OK));
}

static void build_tabs(void)
{
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
    lv_obj_set_size(tabview, 800, 480);

    tab_animals = lv_tabview_add_tab(tabview, ui_get_text(TXT_ANIMALS));
    tab_terrariums = lv_tabview_add_tab(tabview, ui_get_text(TXT_TERRARIUMS));
    lv_obj_t *tab_settings;
    if (logged_role == ROLE_PROFESSIONNEL) {
        tab_stocks = lv_tabview_add_tab(tabview, ui_get_text(TXT_STOCKS));
        tab_transactions = lv_tabview_add_tab(tabview, ui_get_text(TXT_TRANSACTIONS));
        tab_elevages = lv_tabview_add_tab(tabview, ui_get_text(TXT_ELEVAGES));
        tab_settings = lv_tabview_add_tab(tabview, ui_get_text(TXT_SETTINGS));
        animals_tab_create(tab_animals);
        terrariums_tab_create(tab_terrariums);
        stocks_tab_create(tab_stocks);
        transactions_tab_create(tab_transactions);
        elevages_tab_create(tab_elevages);
    } else {
        tab_settings = lv_tabview_add_tab(tabview, ui_get_text(TXT_SETTINGS));
        animals_tab_create(tab_animals);
        terrariums_tab_create(tab_terrariums);
    }
    settings_tab_create(tab_settings);
}

static void animals_tab_create(lv_obj_t *tab)
{
    list_animals = lv_list_create(tab);
    lv_obj_set_size(list_animals, 380, 420);
    for (int i = 0; i < animals_count_for_elevage(current_elevage_id); ++i) {
        const Reptile *r = animals_get_by_index_for_elevage(i, current_elevage_id);
        if (!r) continue;
        const char *status = (legal_is_cerfa_valid(r) && legal_is_cites_valid(r)) ?
                                ui_get_text(TXT_LEGAL_OK) : ui_get_text(TXT_LEGAL_EXPIRED);
        lv_obj_t *btn = lv_list_add_btn(list_animals, NULL, r->name);
        lv_obj_add_event_cb(btn, animal_edit_event, LV_EVENT_CLICKED, (void *)(intptr_t)r->id);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, status);
        lv_obj_align(lbl, LV_ALIGN_RIGHT_MID, -10, 0);
    }
    lv_obj_t *btn_add = lv_btn_create(tab);
    lv_obj_align(btn_add, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_add, animal_add_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn_add), ui_get_text(TXT_ADD));
}

static void terrariums_tab_create(lv_obj_t *tab)
{
    list_terrariums = lv_list_create(tab);
    lv_obj_set_size(list_terrariums, 380, 420);
    for (int i = 0; i < terrariums_count_for_elevage(current_elevage_id); ++i) {
        const Terrarium *t = terrariums_get_by_index_for_elevage(i, current_elevage_id);
        if (!t) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%s (%d)", t->name, t->capacity);
        lv_obj_t *btn = lv_list_add_btn(list_terrariums, NULL, buf);
        lv_obj_add_event_cb(btn, terrarium_edit_event, LV_EVENT_CLICKED, (void *)(intptr_t)t->id);
    }
    lv_obj_t *btn_add = lv_btn_create(tab);
    lv_obj_align(btn_add, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_add, terrarium_add_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn_add), ui_get_text(TXT_ADD));
}

static void stocks_tab_create(lv_obj_t *tab)
{
    list_stocks = lv_list_create(tab);
    lv_obj_set_size(list_stocks, 380, 420);
    for (int i = 0; i < stocks_count(); ++i) {
        const StockItem *s = stocks_get_by_index(i);
        if (!s) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%s (%d)", s->name, s->quantity);
        lv_obj_t *btn = lv_list_add_btn(list_stocks, NULL, buf);
        lv_obj_add_event_cb(btn, stock_edit_event, LV_EVENT_CLICKED, (void *)(intptr_t)s->id);
    }
    lv_obj_t *btn_add = lv_btn_create(tab);
    lv_obj_align(btn_add, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_add, stock_add_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn_add), ui_get_text(TXT_ADD));
}

static void transactions_tab_create(lv_obj_t *tab)
{
    list_transactions = lv_list_create(tab);
    lv_obj_set_size(list_transactions, 380, 420);
    for (int i = 0; i < transactions_count(); ++i) {
        const Transaction *tr = transactions_get_by_index(i);
        if (!tr) continue;
        char buf[64];
        snprintf(buf, sizeof(buf), "%d %s %d", tr->stock_id,
                 tr->type == TRANSACTION_PURCHASE ? "+" : "-",
                 tr->quantity);
        lv_obj_t *btn = lv_list_add_btn(list_transactions, NULL, buf);
        lv_obj_add_event_cb(btn, transaction_edit_event, LV_EVENT_CLICKED, (void *)(intptr_t)tr->id);
    }
    lv_obj_t *btn_add = lv_btn_create(tab);
    lv_obj_align(btn_add, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_add, transaction_add_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn_add), ui_get_text(TXT_ADD));
}

static void elevages_tab_create(lv_obj_t *tab)
{
    list_elevages = lv_list_create(tab);
    lv_obj_set_size(list_elevages, 380, 420);
    for (int i = 0; i < elevages_count(); ++i) {
        const Elevage *ev = elevages_get_by_index(i);
        if (!ev) continue;
        lv_obj_t *btn = lv_list_add_btn(list_elevages, NULL, ev->name);
        lv_obj_add_event_cb(btn, elevage_edit_event, LV_EVENT_CLICKED, (void *)(intptr_t)ev->id);
    }
    lv_obj_t *btn_add = lv_btn_create(tab);
    lv_obj_align(btn_add, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btn_add, elevage_add_event, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn_add), ui_get_text(TXT_ADD));
}

void ui_set_elevage(int elevage_id)
{
    current_elevage_id = elevage_id;
    if (tab_animals)
        refresh_animals();
    if (tab_terrariums)
        refresh_terrariums();
    if (tab_stocks)
        refresh_stocks();
    if (tab_transactions)
        refresh_transactions();
    if (tab_elevages)
        refresh_elevages();
}

int ui_get_elevage(void)
{
    return current_elevage_id;
}

static void refresh_animals(void)
{
    if (tab_animals) {
        lv_obj_clean(tab_animals);
        animals_tab_create(tab_animals);
    }
}

static void refresh_terrariums(void)
{
    if (tab_terrariums) {
        lv_obj_clean(tab_terrariums);
        terrariums_tab_create(tab_terrariums);
    }
}

static void refresh_stocks(void)
{
    if (tab_stocks) {
        lv_obj_clean(tab_stocks);
        stocks_tab_create(tab_stocks);
    }
}

static void refresh_transactions(void)
{
    if (tab_transactions) {
        lv_obj_clean(tab_transactions);
        transactions_tab_create(tab_transactions);
    }
}

static void refresh_elevages(void)
{
    if (tab_elevages) {
        lv_obj_clean(tab_elevages);
        elevages_tab_create(tab_elevages);
    }
}

/* ---- Animal form handlers ---- */

static void animal_save_event(lv_event_t *e)
{
    AnimalFormCtx *ctx = (AnimalFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    Reptile r = {0};
    r.id = atoi(lv_textarea_get_text(ctx->ta_id));
    r.elevage_id = current_elevage_id;
    strncpy(r.name, lv_textarea_get_text(ctx->ta_name), sizeof(r.name) - 1);
    strncpy(r.species, lv_textarea_get_text(ctx->ta_species), sizeof(r.species) - 1);
    strncpy(r.sex, lv_textarea_get_text(ctx->ta_sex), sizeof(r.sex) - 1);
    strncpy(r.birth_date, lv_textarea_get_text(ctx->ta_birth), sizeof(r.birth_date) - 1);
    strncpy(r.cdc_number, lv_textarea_get_text(ctx->ta_cdc), sizeof(r.cdc_number) - 1);
    strncpy(r.aoe_number, lv_textarea_get_text(ctx->ta_aoe), sizeof(r.aoe_number) - 1);
    strncpy(r.ifap_id, lv_textarea_get_text(ctx->ta_ifap), sizeof(r.ifap_id) - 1);
    r.quota_limit = atoi(lv_textarea_get_text(ctx->ta_quota_limit));
    r.quota_used = atoi(lv_textarea_get_text(ctx->ta_quota_used));
    r.cerfa_valid_until = atoi(lv_textarea_get_text(ctx->ta_cerfa));
    r.cites_valid_until = atoi(lv_textarea_get_text(ctx->ta_cites));
    if (ctx->is_new)
        animals_add(&r);
    else
        animals_update(ctx->orig_id, &r);
    lv_obj_del(ctx->win);
    refresh_animals();
}

static void animal_delete_event(lv_event_t *e)
{
    AnimalFormCtx *ctx = (AnimalFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    animals_delete(ctx->orig_id);
    lv_obj_del(ctx->win);
    refresh_animals();
}

static void health_list_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    open_health_list(id);
}

static void breeding_list_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    open_breeding_list(id);
}

static void open_animal_form(const Reptile *r)
{
    animal_form.is_new = (r == NULL);
    animal_form.orig_id = r ? r->id : 0;
    animal_form.win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(animal_form.win, 300, 520);
    lv_obj_center(animal_form.win);

    animal_form.ta_id = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_id, 280);
    lv_textarea_set_placeholder_text(animal_form.ta_id, "ID");
    if (r) {
        char buf[16];
        sprintf(buf, "%d", r->id);
        lv_textarea_set_text(animal_form.ta_id, buf);
    }

    animal_form.ta_name = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_name, 280);
    lv_obj_align(animal_form.ta_name, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(animal_form.ta_name, "Name");
    if (r) lv_textarea_set_text(animal_form.ta_name, r->name);

    animal_form.ta_species = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_species, 280);
    lv_obj_align(animal_form.ta_species, LV_ALIGN_TOP_MID, 0, 80);
    lv_textarea_set_placeholder_text(animal_form.ta_species, "Species");
    if (r) lv_textarea_set_text(animal_form.ta_species, r->species);

    animal_form.ta_sex = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_sex, 280);
    lv_obj_align(animal_form.ta_sex, LV_ALIGN_TOP_MID, 0, 120);
    lv_textarea_set_placeholder_text(animal_form.ta_sex, "Sex");
    if (r) lv_textarea_set_text(animal_form.ta_sex, r->sex);

    animal_form.ta_birth = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_birth, 280);
    lv_obj_align(animal_form.ta_birth, LV_ALIGN_TOP_MID, 0, 160);
    lv_textarea_set_placeholder_text(animal_form.ta_birth, "Birth");
    if (r) lv_textarea_set_text(animal_form.ta_birth, r->birth_date);

    animal_form.ta_cdc = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_cdc, 280);
    lv_obj_align(animal_form.ta_cdc, LV_ALIGN_TOP_MID, 0, 200);
    lv_textarea_set_placeholder_text(animal_form.ta_cdc, ui_get_text(TXT_CDC_NUMBER));
    if (r) lv_textarea_set_text(animal_form.ta_cdc, r->cdc_number);

    animal_form.ta_aoe = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_aoe, 280);
    lv_obj_align(animal_form.ta_aoe, LV_ALIGN_TOP_MID, 0, 240);
    lv_textarea_set_placeholder_text(animal_form.ta_aoe, ui_get_text(TXT_AOE_NUMBER));
    if (r) lv_textarea_set_text(animal_form.ta_aoe, r->aoe_number);

    animal_form.ta_ifap = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_ifap, 280);
    lv_obj_align(animal_form.ta_ifap, LV_ALIGN_TOP_MID, 0, 280);
    lv_textarea_set_placeholder_text(animal_form.ta_ifap, ui_get_text(TXT_IFAP_ID));
    if (r) lv_textarea_set_text(animal_form.ta_ifap, r->ifap_id);

    animal_form.ta_quota_limit = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_quota_limit, 280);
    lv_obj_align(animal_form.ta_quota_limit, LV_ALIGN_TOP_MID, 0, 320);
    lv_textarea_set_placeholder_text(animal_form.ta_quota_limit, ui_get_text(TXT_QUOTA_LIMIT));
    if (r) { char buf[16]; sprintf(buf, "%d", r->quota_limit); lv_textarea_set_text(animal_form.ta_quota_limit, buf); }

    animal_form.ta_quota_used = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_quota_used, 280);
    lv_obj_align(animal_form.ta_quota_used, LV_ALIGN_TOP_MID, 0, 360);
    lv_textarea_set_placeholder_text(animal_form.ta_quota_used, ui_get_text(TXT_QUOTA_USED));
    if (r) { char buf[16]; sprintf(buf, "%d", r->quota_used); lv_textarea_set_text(animal_form.ta_quota_used, buf); }

    animal_form.ta_cerfa = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_cerfa, 280);
    lv_obj_align(animal_form.ta_cerfa, LV_ALIGN_TOP_MID, 0, 400);
    lv_textarea_set_placeholder_text(animal_form.ta_cerfa, ui_get_text(TXT_CERFA_EXPIRY));
    if (r) { char buf[16]; sprintf(buf, "%d", r->cerfa_valid_until); lv_textarea_set_text(animal_form.ta_cerfa, buf); }

    animal_form.ta_cites = lv_textarea_create(animal_form.win);
    lv_obj_set_width(animal_form.ta_cites, 280);
    lv_obj_align(animal_form.ta_cites, LV_ALIGN_TOP_MID, 0, 440);
    lv_textarea_set_placeholder_text(animal_form.ta_cites, ui_get_text(TXT_CITES_EXPIRY));
    if (r) { char buf[16]; sprintf(buf, "%d", r->cites_valid_until); lv_textarea_set_text(animal_form.ta_cites, buf); }

    lv_obj_t *btn_save = lv_btn_create(animal_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, animal_save_event, LV_EVENT_CLICKED, &animal_form);
    lv_label_set_text(lv_label_create(btn_save), ui_get_text(TXT_SAVE));

    if (!animal_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(animal_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, animal_delete_event, LV_EVENT_CLICKED, &animal_form);
        lv_label_set_text(lv_label_create(btn_del), ui_get_text(TXT_DELETE));

        lv_obj_t *btn_health = lv_btn_create(animal_form.win);
        lv_obj_align(btn_health, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_add_event_cb(btn_health, health_list_event, LV_EVENT_CLICKED, (void *)(intptr_t)animal_form.orig_id);
        lv_label_set_text(lv_label_create(btn_health), "Health");

        lv_obj_t *btn_breed = lv_btn_create(animal_form.win);
        lv_obj_align(btn_breed, LV_ALIGN_BOTTOM_MID, 0, -50);
        lv_obj_add_event_cb(btn_breed, breeding_list_event, LV_EVENT_CLICKED, (void *)(intptr_t)animal_form.orig_id);
        lv_label_set_text(lv_label_create(btn_breed), "Breeding");
    }
}

static void animal_add_event(lv_event_t *e)
{
    (void)e;
    open_animal_form(NULL);
}

static void animal_edit_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    const Reptile *r = animals_get(id);
    if (r) open_animal_form(r);
}

/* ---- Terrarium form handlers ---- */

static void terrarium_save_event(lv_event_t *e)
{
    TerrariumFormCtx *ctx = (TerrariumFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    Terrarium t = {0};
    t.id = atoi(lv_textarea_get_text(ctx->ta_id));
    t.elevage_id = current_elevage_id;
    strncpy(t.name, lv_textarea_get_text(ctx->ta_name), sizeof(t.name) - 1);
    t.capacity = atoi(lv_textarea_get_text(ctx->ta_capacity));
    if (ctx->is_new)
        terrariums_add(&t);
    else
        terrariums_update(ctx->orig_id, &t);
    lv_obj_del(ctx->win);
    refresh_terrariums();
}

static void terrarium_delete_event(lv_event_t *e)
{
    TerrariumFormCtx *ctx = (TerrariumFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    terrariums_delete(ctx->orig_id);
    lv_obj_del(ctx->win);
    refresh_terrariums();
}

static void open_terrarium_form(const Terrarium *t)
{
    terrarium_form.is_new = (t == NULL);
    terrarium_form.orig_id = t ? t->id : 0;
    terrarium_form.win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(terrarium_form.win, 300, 200);
    lv_obj_center(terrarium_form.win);

    terrarium_form.ta_id = lv_textarea_create(terrarium_form.win);
    lv_obj_set_width(terrarium_form.ta_id, 280);
    lv_textarea_set_placeholder_text(terrarium_form.ta_id, "ID");
    if (t) { char buf[16]; sprintf(buf, "%d", t->id); lv_textarea_set_text(terrarium_form.ta_id, buf); }

    terrarium_form.ta_name = lv_textarea_create(terrarium_form.win);
    lv_obj_set_width(terrarium_form.ta_name, 280);
    lv_obj_align(terrarium_form.ta_name, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(terrarium_form.ta_name, "Name");
    if (t) lv_textarea_set_text(terrarium_form.ta_name, t->name);

    terrarium_form.ta_capacity = lv_textarea_create(terrarium_form.win);
    lv_obj_set_width(terrarium_form.ta_capacity, 280);
    lv_obj_align(terrarium_form.ta_capacity, LV_ALIGN_TOP_MID, 0, 80);
    lv_textarea_set_placeholder_text(terrarium_form.ta_capacity, "Capacity");
    if (t) { char buf[16]; sprintf(buf, "%d", t->capacity); lv_textarea_set_text(terrarium_form.ta_capacity, buf); }

    lv_obj_t *btn_save = lv_btn_create(terrarium_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, terrarium_save_event, LV_EVENT_CLICKED, &terrarium_form);
    lv_label_set_text(lv_label_create(btn_save), ui_get_text(TXT_SAVE));

    if (!terrarium_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(terrarium_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, terrarium_delete_event, LV_EVENT_CLICKED, &terrarium_form);
        lv_label_set_text(lv_label_create(btn_del), ui_get_text(TXT_DELETE));
    }
}

static void terrarium_add_event(lv_event_t *e)
{
    (void)e;
    open_terrarium_form(NULL);
}

static void terrarium_edit_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    const Terrarium *t = terrariums_get(id);
    if (t) open_terrarium_form(t);
}

/* ---- Stock form handlers ---- */

static void stock_save_event(lv_event_t *e)
{
    StockFormCtx *ctx = (StockFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    StockItem s = {0};
    s.id = atoi(lv_textarea_get_text(ctx->ta_id));
    strncpy(s.name, lv_textarea_get_text(ctx->ta_name), sizeof(s.name) - 1);
    s.quantity = atoi(lv_textarea_get_text(ctx->ta_qty));
    s.min_quantity = atoi(lv_textarea_get_text(ctx->ta_min));
    if (ctx->is_new)
        stocks_add(&s);
    else
        stocks_update(ctx->orig_id, &s);
    lv_obj_del(ctx->win);
    refresh_stocks();
}

static void stock_delete_event(lv_event_t *e)
{
    StockFormCtx *ctx = (StockFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    stocks_delete(ctx->orig_id);
    lv_obj_del(ctx->win);
    refresh_stocks();
}

static void open_stock_form(const StockItem *s)
{
    stock_form.is_new = (s == NULL);
    stock_form.orig_id = s ? s->id : 0;
    stock_form.win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(stock_form.win, 300, 200);
    lv_obj_center(stock_form.win);

    stock_form.ta_id = lv_textarea_create(stock_form.win);
    lv_obj_set_width(stock_form.ta_id, 280);
    lv_textarea_set_placeholder_text(stock_form.ta_id, "ID");
    if (s) { char buf[16]; sprintf(buf, "%d", s->id); lv_textarea_set_text(stock_form.ta_id, buf); }

    stock_form.ta_name = lv_textarea_create(stock_form.win);
    lv_obj_set_width(stock_form.ta_name, 280);
    lv_obj_align(stock_form.ta_name, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(stock_form.ta_name, "Name");
    if (s) lv_textarea_set_text(stock_form.ta_name, s->name);

    stock_form.ta_qty = lv_textarea_create(stock_form.win);
    lv_obj_set_width(stock_form.ta_qty, 280);
    lv_obj_align(stock_form.ta_qty, LV_ALIGN_TOP_MID, 0, 80);
    lv_textarea_set_placeholder_text(stock_form.ta_qty, "Quantity");
    if (s) { char buf[16]; sprintf(buf, "%d", s->quantity); lv_textarea_set_text(stock_form.ta_qty, buf); }

    stock_form.ta_min = lv_textarea_create(stock_form.win);
    lv_obj_set_width(stock_form.ta_min, 280);
    lv_obj_align(stock_form.ta_min, LV_ALIGN_TOP_MID, 0, 120);
    lv_textarea_set_placeholder_text(stock_form.ta_min, "Min");
    if (s) { char buf[16]; sprintf(buf, "%d", s->min_quantity); lv_textarea_set_text(stock_form.ta_min, buf); }

    lv_obj_t *btn_save = lv_btn_create(stock_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, stock_save_event, LV_EVENT_CLICKED, &stock_form);
    lv_label_set_text(lv_label_create(btn_save), ui_get_text(TXT_SAVE));

    if (!stock_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(stock_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, stock_delete_event, LV_EVENT_CLICKED, &stock_form);
        lv_label_set_text(lv_label_create(btn_del), ui_get_text(TXT_DELETE));
    }
}

static void stock_add_event(lv_event_t *e)
{
    (void)e;
    open_stock_form(NULL);
}

static void stock_edit_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    const StockItem *s = stocks_get(id);
    if (s) open_stock_form(s);
}

/* ---- Transaction form handlers ---- */

static void transaction_save_event(lv_event_t *e)
{
    TransactionFormCtx *ctx = (TransactionFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    Transaction t = {0};
    t.id = atoi(lv_textarea_get_text(ctx->ta_id));
    t.stock_id = atoi(lv_textarea_get_text(ctx->ta_stock_id));
    t.quantity = atoi(lv_textarea_get_text(ctx->ta_qty));
    t.type = lv_dropdown_get_selected(ctx->dd_type) == 0 ? TRANSACTION_PURCHASE : TRANSACTION_SALE;
    if (ctx->is_new)
        transactions_add(&t);
    else
        transactions_update(ctx->orig_id, &t);
    lv_obj_del(ctx->win);
    refresh_transactions();
}

static void transaction_delete_event(lv_event_t *e)
{
    TransactionFormCtx *ctx = (TransactionFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    transactions_delete(ctx->orig_id);
    lv_obj_del(ctx->win);
    refresh_transactions();
}

static void open_transaction_form(const Transaction *t)
{
    transaction_form.is_new = (t == NULL);
    transaction_form.orig_id = t ? t->id : 0;
    transaction_form.win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(transaction_form.win, 300, 220);
    lv_obj_center(transaction_form.win);

    transaction_form.ta_id = lv_textarea_create(transaction_form.win);
    lv_obj_set_width(transaction_form.ta_id, 280);
    lv_textarea_set_placeholder_text(transaction_form.ta_id, "ID");
    if (t) { char buf[16]; sprintf(buf, "%d", t->id); lv_textarea_set_text(transaction_form.ta_id, buf); }

    transaction_form.ta_stock_id = lv_textarea_create(transaction_form.win);
    lv_obj_set_width(transaction_form.ta_stock_id, 280);
    lv_obj_align(transaction_form.ta_stock_id, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(transaction_form.ta_stock_id, "Stock ID");
    if (t) { char buf[16]; sprintf(buf, "%d", t->stock_id); lv_textarea_set_text(transaction_form.ta_stock_id, buf); }

    transaction_form.ta_qty = lv_textarea_create(transaction_form.win);
    lv_obj_set_width(transaction_form.ta_qty, 280);
    lv_obj_align(transaction_form.ta_qty, LV_ALIGN_TOP_MID, 0, 80);
    lv_textarea_set_placeholder_text(transaction_form.ta_qty, "Quantity");
    if (t) { char buf[16]; sprintf(buf, "%d", t->quantity); lv_textarea_set_text(transaction_form.ta_qty, buf); }

    transaction_form.dd_type = lv_dropdown_create(transaction_form.win);
    lv_obj_set_width(transaction_form.dd_type, 280);
    lv_obj_align(transaction_form.dd_type, LV_ALIGN_TOP_MID, 0, 120);
    lv_dropdown_set_options(transaction_form.dd_type, "Purchase\nSale");
    if (t) lv_dropdown_set_selected(transaction_form.dd_type, t->type == TRANSACTION_PURCHASE ? 0 : 1);

    lv_obj_t *btn_save = lv_btn_create(transaction_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, transaction_save_event, LV_EVENT_CLICKED, &transaction_form);
    lv_label_set_text(lv_label_create(btn_save), ui_get_text(TXT_SAVE));

    if (!transaction_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(transaction_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, transaction_delete_event, LV_EVENT_CLICKED, &transaction_form);
        lv_label_set_text(lv_label_create(btn_del), ui_get_text(TXT_DELETE));
    }
}

static void transaction_add_event(lv_event_t *e)
{
    (void)e;
    open_transaction_form(NULL);
}

static void transaction_edit_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    const Transaction *t = transactions_get(id);
    if (t) open_transaction_form(t);
}

/* ---- Elevage form handlers ---- */

static void elevage_save_event(lv_event_t *e)
{
    ElevageFormCtx *ctx = (ElevageFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    Elevage ev = {0};
    ev.id = atoi(lv_textarea_get_text(ctx->ta_id));
    strncpy(ev.name, lv_textarea_get_text(ctx->ta_name), sizeof(ev.name) - 1);
    strncpy(ev.description, lv_textarea_get_text(ctx->ta_desc), sizeof(ev.description) - 1);
    if (ctx->is_new)
        elevages_add(&ev);
    else
        elevages_update(ctx->orig_id, &ev);
    lv_obj_del(ctx->win);
    refresh_elevages();
}

static void elevage_delete_event(lv_event_t *e)
{
    ElevageFormCtx *ctx = (ElevageFormCtx *)lv_event_get_user_data(e);
    if (!ctx) return;
    elevages_delete(ctx->orig_id);
    lv_obj_del(ctx->win);
    refresh_elevages();
}

static void open_elevage_form(const Elevage *ev)
{
    elevage_form.is_new = (ev == NULL);
    elevage_form.orig_id = ev ? ev->id : 0;
    elevage_form.win = lv_obj_create(lv_scr_act());
    lv_obj_set_size(elevage_form.win, 300, 180);
    lv_obj_center(elevage_form.win);

    elevage_form.ta_id = lv_textarea_create(elevage_form.win);
    lv_obj_set_width(elevage_form.ta_id, 280);
    lv_textarea_set_placeholder_text(elevage_form.ta_id, "ID");
    if (ev) { char buf[16]; sprintf(buf, "%d", ev->id); lv_textarea_set_text(elevage_form.ta_id, buf); }

    elevage_form.ta_name = lv_textarea_create(elevage_form.win);
    lv_obj_set_width(elevage_form.ta_name, 280);
    lv_obj_align(elevage_form.ta_name, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(elevage_form.ta_name, "Name");
    if (ev) lv_textarea_set_text(elevage_form.ta_name, ev->name);

    elevage_form.ta_desc = lv_textarea_create(elevage_form.win);
    lv_obj_set_width(elevage_form.ta_desc, 280);
    lv_obj_align(elevage_form.ta_desc, LV_ALIGN_TOP_MID, 0, 80);
    lv_textarea_set_placeholder_text(elevage_form.ta_desc, "Description");
    if (ev) lv_textarea_set_text(elevage_form.ta_desc, ev->description);

    lv_obj_t *btn_save = lv_btn_create(elevage_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, elevage_save_event, LV_EVENT_CLICKED, &elevage_form);
    lv_label_set_text(lv_label_create(btn_save), ui_get_text(TXT_SAVE));

    if (!elevage_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(elevage_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, elevage_delete_event, LV_EVENT_CLICKED, &elevage_form);
        lv_label_set_text(lv_label_create(btn_del), ui_get_text(TXT_DELETE));
    }
}

static void elevage_add_event(lv_event_t *e)
{
    (void)e;
    open_elevage_form(NULL);
}

static void elevage_edit_event(lv_event_t *e)
{
    int id = (int)(intptr_t)lv_event_get_user_data(e);
    const Elevage *ev = elevages_get(id);
    if (ev) open_elevage_form(ev);
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
        int idx = 0;
        lv_tabview_set_tab_name(tabview, idx++, ui_get_text(TXT_ANIMALS));
        lv_tabview_set_tab_name(tabview, idx++, ui_get_text(TXT_TERRARIUMS));
        if (logged_role == ROLE_PROFESSIONNEL) {
            lv_tabview_set_tab_name(tabview, idx++, ui_get_text(TXT_STOCKS));
            lv_tabview_set_tab_name(tabview, idx++, ui_get_text(TXT_TRANSACTIONS));
            lv_tabview_set_tab_name(tabview, idx++, ui_get_text(TXT_ELEVAGES));
        }
        lv_tabview_set_tab_name(tabview, idx, ui_get_text(TXT_SETTINGS));
    }
    if (login_win) {
        lv_textarea_set_placeholder_text(ta_user, ui_get_text(TXT_USERNAME));
        lv_textarea_set_placeholder_text(ta_pass, ui_get_text(TXT_PASSWORD));
        lv_label_set_text(lbl_login, ui_get_text(TXT_LOGIN));
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
    create_login();
}

void ui_notify(const char *msg)
{
    if (!notif_label || !msg)
        return;
    lv_label_set_text(notif_label, msg);
}
