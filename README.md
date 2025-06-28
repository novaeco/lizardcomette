# LizardComette

LizardComette est un exemple de projet ESP‑IDF permettant de gérer un élevage de reptiles via une interface graphique LVGL. Il intègre des modules pour la base de données, l'authentification et la génération de documents légaux. Ce dépôt est fourni pour étude et nécessite une adaptation avant toute utilisation en production.

## Fonctionnalités principales
- Gestion des animaux, des terrariums, du stock et des transactions.
- Authentification multi-utilisateur avec mots de passe hachés et rôles.
- Séparation des données par identifiant d'élevage pour les animaux et terrariums.
- Drivers I2C/SPI pour capteurs environnementaux avec envoi REST/MQTT.
- Planificateur avec notifications (stocks, échéances, conformité).
- Génération de formulaires administratifs simplifiés.
- Support du CDC/AOE, gestion des quotas et formulaires CERFA/CITES officiels.

## Configuration
1. Installez l'[ESP‑IDF](https://docs.espressif.com/).
2. Définissez la variable `IDF_PATH` correspondant à votre installation.
3. Inspirez‑vous de `docs/CONFIG_EXAMPLE.md` pour créer votre propre `sdkconfig` (Wi‑Fi, stockage, REST/MQTT, etc.).
4. Placez vos fichiers de licences CITES et autres documents dans le répertoire approprié.

## Compilation
```bash
idf.py set-target esp32
idf.py build
```

## Utilisation
Une fois flashé sur votre ESP32, le firmware démarre l'interface graphique en français ou en anglais selon la configuration. Les modules s'initialisent automatiquement puis le planificateur vérifie les tâches à venir. Consultez `docs/UI_USAGE.md` pour le détail des écrans et `docs/NOTICE.md` pour les avertissements légaux.

## Obligations et responsabilités
Vous êtes seul responsable de la conformité des documents générés et du respect de la législation locale (France, Europe, international). Les exemples fournis ne constituent pas un conseil juridique et peuvent requérir l'avis d'un professionnel avant usage.

## Licence
Ce projet est distribué sous la licence MIT. Voir le fichier `LICENSE` pour plus de détails.
