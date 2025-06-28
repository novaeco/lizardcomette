#include "unity.h"
#include <string.h>

typedef enum { UI_LANG_EN, UI_LANG_FR, UI_LANG_COUNT } ui_language_t;
typedef enum { UI_THEME_LIGHT, UI_THEME_DARK, UI_THEME_COUNT } ui_theme_t;
typedef enum {
    TXT_ANIMALS,
    TXT_TERRARIUMS,
    TXT_STOCKS,
    TXT_TRANSACTIONS,
    TXT_ELEVAGES,
    TXT_SETTINGS,
    TXT_EXPORT,
    TXT_IMPORT,
    TXT_LEGAL_OK,
    TXT_LEGAL_EXPIRED,
    TXT_LANGUAGE,
    TXT_THEME,
    TXT_LOGIN,
    TXT_USERNAME,
    TXT_PASSWORD,
    TXT_ADD,
    TXT_SAVE,
    TXT_DELETE,
    TXT_OK,
    TXT_CDC_NUMBER,
    TXT_AOE_NUMBER,
    TXT_IFAP_ID,
    TXT_QUOTA_LIMIT,
    TXT_QUOTA_USED,
    TXT_CERFA_EXPIRY,
    TXT_CITES_EXPIRY,
    TXT_COUNT
} ui_text_id_t;

static ui_language_t current_lang;
static ui_theme_t current_theme;

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

void ui_init(ui_language_t lang, ui_theme_t theme)
{
    current_lang = lang;
    current_theme = theme;
}

void ui_set_language(ui_language_t lang)
{
    current_lang = lang;
}

void ui_set_theme(ui_theme_t theme)
{
    current_theme = theme;
}

const char *ui_get_text(ui_text_id_t id)
{
    return translations[current_lang][id];
}

TEST_CASE("ui language switching","[ui]")
{
    ui_init(UI_LANG_EN, UI_THEME_LIGHT);
    TEST_ASSERT_EQUAL_STRING("Animals", ui_get_text(TXT_ANIMALS));
    ui_set_language(UI_LANG_FR);
    TEST_ASSERT_EQUAL_STRING("Animaux", ui_get_text(TXT_ANIMALS));
}

TEST_CASE("ui theme switching","[ui]")
{
    ui_init(UI_LANG_EN, UI_THEME_LIGHT);
    ui_set_theme(UI_THEME_DARK);
    TEST_ASSERT_EQUAL_INT(UI_THEME_DARK, current_theme);
}
