# LizardComette

LizardComette est un exemple de projet ESP‑IDF permettant de gérer un élevage de reptiles via une interface graphique LVGL. Il intègre des modules pour la base de données, l'authentification et la génération de documents légaux. Ce dépôt est fourni pour étude et nécessite une adaptation avant toute utilisation en production.

## Fonctionnalités principales
- Gestion des animaux, des terrariums, du stock et des transactions.
- Authentification multi-utilisateur avec mots de passe hachés et rôles.
- Séparation des données par identifiant d'élevage pour les animaux et terrariums.
- Drivers I2C/SPI pour capteurs environnementaux (température, humidité, CO₂) et éclairage avec envoi REST/MQTT.
- Planificateur avec notifications (stocks, échéances, conformité).
- Génération de formulaires administratifs via superposition sur les
  modèles officiels CERFA et CITES.
- Support du CDC/AOE, gestion des quotas et alertes d'expiration
  programmées.

## Configuration
1. Installez l'[ESP‑IDF](https://docs.espressif.com/).
2. Définissez la variable `IDF_PATH` correspondant à votre installation.
3. Inspirez‑vous de `docs/CONFIG_EXAMPLE.md` pour créer votre propre `sdkconfig` (Wi‑Fi, stockage, REST/MQTT, etc.).
4. Placez vos fichiers de licences CITES et autres documents dans le répertoire approprié.
5. Ajustez `partition_table.csv` si vous avez besoin d'une taille différente pour les partitions SPIFFS ou SD afin de stocker la base de données.
6. Pour envoyer automatiquement les sauvegardes sur un serveur HTTP, renseignez
   `CONFIG_STORAGE_TRANSFER_URL` dans `sdkconfig`. Si votre serveur exige une
   authentification, complétez également `CONFIG_STORAGE_TRANSFER_USERNAME` et
   `CONFIG_STORAGE_TRANSFER_PASSWORD`.

## Installation des composants tiers
Certaines bibliothèques ne sont pas incluses dans ce dépôt et doivent être téléchargées via le registre d'Espressif. Utilisez `idf.py add-dependency` pour installer chaque composant indiqué dans `idf_component.yml`. Par exemple, le projet requiert le composant `sqlite3` :

```bash
idf.py add-dependency "espressif/sqlite3"
```

Vous pouvez aussi copier manuellement les composants dans le dossier `components/`.

## Compilation
```bash
idf.py add-dependency "espressif/sqlite3"
idf.py set-target esp32s3
idf.py build
```
Le projet a besoin du composant `sqlite3` fourni par l'ESP‑IDF. Celui‑ci est
déclaré dans `idf_component.yml` sous le nom `espressif/sqlite3` et sera
téléchargé automatiquement. Exécutez une fois la commande
`idf.py add-dependency "espressif/sqlite3"` pour l'installer, ou placez le
composant dans le répertoire `components/`.

## Tests
Les tests unitaires basés sur Unity se trouvent dans le dossier `tests/`. Après
configuration de l'ESP‑IDF, lancez la compilation puis l'exécution des tests
avec&nbsp;:

```bash
idf.py build && idf.py unity_test
```

Ces commandes sont aussi utilisées par le workflow CI
[`ci.yml`](.github/workflows/ci.yml) pour valider chaque contribution.

## Utilisation
Une fois flashé sur votre ESP32, le firmware démarre l'interface graphique en français ou en anglais selon la configuration. Les modules s'initialisent automatiquement puis le planificateur vérifie les tâches à venir. Consultez `docs/UI_USAGE.md` pour le détail des écrans et `docs/NOTICE.md` pour les avertissements légaux.

## Gestion des numéros CDC/AOE
Les numéros d'autorisation (CDC et AOE) ne sont plus codés en dur. Ils sont stockés dans la
table `cdc_aoe_numbers`. Chaque entrée possède un `id`, éventuellement un `username`,
un `elevage_id`, un `type` (`CDC` ou `AOE`) et le `number`. Utilisez les fonctions de
`legal_numbers.h` pour manipuler ces listes et ajoutez vos valeurs pour que
`legal_is_cdc_valid()` et `legal_is_aoe_valid()` fonctionnent correctement.

## Base de données chiffrée
Si SQLCipher est disponible, la base `lizard.db` est chiffrée. Au premier
démarrage, si aucune clef n'est encore enregistrée dans l'espace NVS "db",
le firmware vous invite à saisir un mot de passe. La valeur indiquée dans
`CONFIG_DB_DEFAULT_KEY` sert alors de proposition initiale : si elle est
définie dans votre `sdkconfig`, elle est utilisée par défaut mais vous
pouvez la modifier à la saisie.

Une fois cette étape terminée, la clef choisie est conservée dans le NVS et
réutilisée à chaque redémarrage. Elle peut être changée à tout moment en
appelant `db_set_key()` depuis votre application.

## Obligations et responsabilités
Vous êtes seul responsable de la conformité des documents générés et du respect de la législation locale (France, Europe, international). Les exemples fournis ne constituent pas un conseil juridique et peuvent requérir l'avis d'un professionnel avant usage.

## Licence
Ce projet est distribué sous la licence MIT. Voir le fichier `LICENSE` pour plus de détails.
