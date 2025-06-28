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
        [TXT_LOGIN] = "Connexion",
        [TXT_USERNAME] = "Utilisateur",
        [TXT_PASSWORD] = "Mot de passe",
    }
};

static lv_obj_t *tabview;
static lv_obj_t *notif_label;
static lv_obj_t *tab_animals;
static lv_obj_t *tab_terrariums;
static lv_obj_t *tab_stocks;
static lv_obj_t *tab_transactions;
static lv_obj_t *list_animals;
static lv_obj_t *list_terrariums;
static lv_obj_t *list_stocks;
static lv_obj_t *list_transactions;

typedef struct {
    lv_obj_t *win;
    lv_obj_t *ta_id;
    lv_obj_t *ta_name;
    lv_obj_t *ta_species;
    lv_obj_t *ta_sex;
    lv_obj_t *ta_birth;
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
static lv_obj_t *login_win;
static lv_obj_t *ta_user;
static lv_obj_t *ta_pass;
static lv_obj_t *btn_login;
static lv_obj_t *lbl_login;
static char logged_user[32];
static user_role_t logged_role;
static bool logged_in = false;

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
        build_tabs();
        notif_label = lv_label_create(lv_scr_act());
        lv_obj_align(notif_label, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_label_set_text(notif_label, "");
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
        tab_settings = lv_tabview_add_tab(tabview, ui_get_text(TXT_SETTINGS));
        animals_tab_create(tab_animals);
        terrariums_tab_create(tab_terrariums);
        stocks_tab_create(tab_stocks);
        transactions_tab_create(tab_transactions);
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
    lv_label_set_text(lv_label_create(btn_add), "Add");
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
    lv_label_set_text(lv_label_create(btn_add), "Add");
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
    lv_label_set_text(lv_label_create(btn_add), "Add");
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
    lv_label_set_text(lv_label_create(btn_add), "Add");
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
    lv_obj_set_size(animal_form.win, 300, 250);
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

    lv_obj_t *btn_save = lv_btn_create(animal_form.win);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btn_save, animal_save_event, LV_EVENT_CLICKED, &animal_form);
    lv_label_set_text(lv_label_create(btn_save), "Save");

    if (!animal_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(animal_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, animal_delete_event, LV_EVENT_CLICKED, &animal_form);
        lv_label_set_text(lv_label_create(btn_del), "Delete");

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
    lv_label_set_text(lv_label_create(btn_save), "Save");

    if (!terrarium_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(terrarium_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, terrarium_delete_event, LV_EVENT_CLICKED, &terrarium_form);
        lv_label_set_text(lv_label_create(btn_del), "Delete");
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
    lv_label_set_text(lv_label_create(btn_save), "Save");

    if (!stock_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(stock_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, stock_delete_event, LV_EVENT_CLICKED, &stock_form);
        lv_label_set_text(lv_label_create(btn_del), "Delete");
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
    lv_label_set_text(lv_label_create(btn_save), "Save");

    if (!transaction_form.is_new) {
        lv_obj_t *btn_del = lv_btn_create(transaction_form.win);
        lv_obj_align(btn_del, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_del, transaction_delete_event, LV_EVENT_CLICKED, &transaction_form);
        lv_label_set_text(lv_label_create(btn_del), "Delete");
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
