# Utilisation de l'interface graphique

Cette application utilise LVGL pour afficher une interface en onglets adaptée aux écrans 800x480.

## Initialisation

```c
ui_init(UI_LANG_EN, UI_THEME_LIGHT);
```

- **Langue** : `UI_LANG_EN` pour l'anglais ou `UI_LANG_FR` pour le français.
- **Thème** : `UI_THEME_LIGHT` ou `UI_THEME_DARK` pour le mode clair ou sombre.

## Changement de langue et de thème

```c
ui_set_language(UI_LANG_FR);
ui_set_theme(UI_THEME_DARK);
```

Les textes des onglets sont mis à jour automatiquement selon la langue sélectionnée.
