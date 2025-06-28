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

## Connexion

Au démarrage, un formulaire de connexion demande l'utilisateur et le mot de passe.
Les identifiants sont vérifiés via `auth_check()`.
Selon le rôle retourné par `auth_get_role()`, certains onglets sont affichés :

- Rôle particulier : seulement "Animaux", "Terrariums" et "Paramètres".
- Rôle professionnel : accès supplémentaire aux onglets "Stocks" et "Transactions".

## Onglets disponibles

- **Animaux** : liste les reptiles avec l'état de leurs documents légaux.
- **Terrariums** : affiche les terrariums disponibles.
- **Stocks** : permet de consulter le matériel ou la nourriture en stock.
- **Transactions** : liste les mouvements de stock.
- **Paramètres** : choix de la langue et du thème, export et import des données.
