# LizardComette

LizardComette est un exemple de projet ESP‑IDF permettant de gérer un élevage de reptiles via une interface graphique LVGL. Il intègre des modules pour la base de données, l'authentification et la génération de documents légaux. Ce dépôt est fourni pour étude et nécessite une adaptation avant toute utilisation en production.

## Fonctionnalités principales
- Gestion des animaux, des terrariums, du stock et des transactions.
- Authentification multi-utilisateur avec mots de passe hachés et rôles.
- Séparation des données par identifiant d'élevage pour les animaux et terrariums.
- Drivers I2C/SPI pour capteurs environnementaux (température, humidité, luminosité, CO₂) et éclairage avec envoi REST/MQTT.
- Planificateur avec notifications (stocks, capteurs, échéances, conformité).
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
Certaines bibliothèques ne sont pas incluses dans ce dépôt et doivent être téléchargées via le registre d'Espressif ([components.espressif.com](https://components.espressif.com)), en utilisant le gestionnaire de composants ESP‑IDF (component manager). Utilisez `idf.py add-dependency` pour installer chaque composant indiqué dans `idf_component.yml`. Par exemple, le projet requiert le composant `sqlite3`, disponible sur [components.espressif.com/components/espressif/sqlite3](https://components.espressif.com/components/espressif/sqlite3) :

```bash
idf.py add-dependency "espressif/sqlite3"
```

Vous pouvez aussi copier manuellement les composants dans le dossier `components/`.

## Compilation
Avant la première compilation, téléchargez obligatoirement le composant `sqlite3` depuis le registre d'Espressif avec `idf.py add-dependency`, puis lancez la
construction du projet&nbsp;:
```bash
idf.py add-dependency "espressif/sqlite3^3"
idf.py set-target esp32s3
idf.py build
```
Le projet a besoin du composant `sqlite3` fourni par l'ESP‑IDF. Celui‑ci est
déclaré dans `idf_component.yml` sous le nom `espressif/sqlite3` et sera
téléchargé automatiquement. Exécutez `idf.py create-component-manifest` ou `idf.py add-dependency "espressif/sqlite3^3"` pour générer `dependencies.lock` et installer la dépendance, ou placez le composant dans le répertoire `components/`.

## Mise en route
Ce petit tutoriel résume les étapes nécessaires lors de la première
compilation et du flashage&nbsp;:

1. Préparez l'environnement ESP‑IDF et ouvrez un terminal dans ce dépôt.
2. Récupérez les dépendances déclarées dans `idf_component.yml`&nbsp;:
   ```bash
   idf.py add-dependency "espressif/sqlite3^3"
   ```
3. Sélectionnez la cible puis construisez le projet&nbsp;:
   ```bash
   idf.py set-target esp32s3
   idf.py build
   ```
4. Branchez votre carte et flashez le firmware (remplacez le port
   par celui de votre système)&nbsp;:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```
5. Téléchargez les formulaires CERFA et CITES mentionnés dans
   `docs/LEGAL_TEMPLATES.md` puis copiez-les dans le dossier `forms/`
   en conservant les noms fournis.

## Tests
L'ensemble des tests unitaires écrits avec Unity est regroupé dans le
répertoire [`tests/`](tests/). Après avoir configuré l'ESP‑IDF, compilez le
projet puis exécutez les tests avec&nbsp;:

```bash
idf.py build
idf.py unity_test
```

Cette procédure est également employée par le workflow CI
[`.github/workflows/ci.yml`](.github/workflows/ci.yml) pour valider chaque
contribution.

Si c'est votre première compilation, exécutez également :

```bash
idf.py add-dependency "espressif/sqlite3^3"
```

Ceci télécharge le composant *SQLite3* et crée le fichier `dependencies.lock` nécessaire.
N'oubliez pas d'ajouter ce fichier au contrôle de version afin d'éviter de
télécharger à nouveau les dépendances lors des prochaines compilations.

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
