# Format de chiffrement des fichiers

Les fichiers produits par `storage_encrypt_file` commencent par un en‑tête
permettant à `storage_decrypt_file` de récupérer les informations nécessaires
au déchiffrement.

```
+-----------+--------------------+----------------------+
| 4 octets  | 16 octets          | Données chiffrées    |
| longueur  | vecteur d'initial. | AES-CBC (paddée)     |
+-----------+--------------------+----------------------+
```

- **longueur** : taille originale du fichier avant chiffrement, little‑endian.
- **vecteur d'initialisation** : IV aléatoire généré pour chaque fichier.
- **données chiffrées** : contenu du fichier chiffré en AES‑128‑CBC et
  aligné sur 16 octets par bourrage nul.

La clé AES de 128 bits n'est pas stockée dans le fichier. Elle doit être
récupérée dans l'espace NVS "storage" sous la clé `aes_key`. Elle peut être
initialisée ou mise à jour via `storage_set_aes_key()`.

## Chiffrement de la base de données

Lorsque SQLCipher est disponible dans l'ESP-IDF, `lizard.db` est chiffrée de
manière transparente. La clef est enregistrée dans l'espace NVS "db" sous la
clé `key`. Elle peut être définie lors du premier démarrage via la console ou
la valeur `CONFIG_DB_DEFAULT_KEY` dans `sdkconfig`. La clef peut être modifiée
par programmation avec `db_set_key()`.

