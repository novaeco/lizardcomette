# LizardComette

Projet ESP-IDF pour la gestion complète d'un élevage de reptiles. Ce dépôt fournit une structure de base pour développer une application embarquée sur ESP32 avec interface graphique LVGL et génération de documents légaux français et internationaux.

## Structure
- `main/` : point d'entrée de l'application.
- `components/` : modules fonctionnels (base de données, UI, authentification,
  gestion des animaux, des terrariums, **drivers de capteurs**, un **planificateur**
  et génération de documents légaux).
  - `scheduler/` : planificateur émettant des notifications sur les échéances,
    le stock et la conformité.
  Les mots de passe sont hachés en SHA‑256 et les fichiers exportés sont chiffrés.
- `docs/` : documentation légale et guides d'utilisation (voir `docs/UI_USAGE.md` pour l'interface).

## Compilation
1. Installer l'[ESP-IDF](https://docs.espressif.com/).
2. Configurer l'environnement `IDF_PATH`.
3. Lancer `idf.py set-target esp32` puis `idf.py build`.

## Avertissement
Ce projet est fourni à titre d'exemple. Il ne garantit pas la conformité totale avec la législation. L'utilisateur doit vérifier chaque document généré.
