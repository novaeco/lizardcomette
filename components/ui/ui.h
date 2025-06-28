#ifndef UI_H
#define UI_H

#include "lvgl.h"

/**
 * \brief Langues supportées.
 */
typedef enum {
    UI_LANG_EN, /**< Anglais */
    UI_LANG_FR, /**< Français */
    UI_LANG_COUNT
} ui_language_t;

/**
 * \brief Thèmes disponibles.
 */
typedef enum {
    UI_THEME_LIGHT, /**< Mode clair */
    UI_THEME_DARK,  /**< Mode sombre */
    UI_THEME_COUNT
} ui_theme_t;

/**
 * \brief Identifiants de texte.
 */
typedef enum {
    TXT_ANIMALS,
    TXT_TERRARIUMS,
    TXT_STOCKS,
    TXT_TRANSACTIONS,
    TXT_SETTINGS,
    TXT_EXPORT,
    TXT_IMPORT,
    TXT_LEGAL_OK,
    TXT_LEGAL_EXPIRED,
    TXT_LANGUAGE,
    TXT_THEME,
    TXT_COUNT
} ui_text_id_t;

/**
 * \brief Initialise l'interface graphique avec langue et thème.
 */
void ui_init(ui_language_t lang, ui_theme_t theme);

/**
 * \brief Change la langue de l'interface.
 */
void ui_set_language(ui_language_t lang);

/**
 * \brief Change le thème (clair/sombre).
 */
void ui_set_theme(ui_theme_t theme);

/**
 * \brief Récupère le texte traduit.
 */
const char *ui_get_text(ui_text_id_t id);

/**
 * \brief Affiche une notification temporaire.
 */
void ui_notify(const char *msg);

#endif // UI_H
