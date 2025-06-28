# Gestion des numéros CDC/AOE

Les numéros utilisés pour vérifier la validité des documents sont stockés dans la table SQLite `cdc_aoe_numbers`.
Chaque enregistrement contient :

- `id` : identifiant unique
- `username` : optionnel, pour restreindre un numéro à un utilisateur
- `elevage_id` : optionnel (0 pour tous)
- `type` : `CDC` ou `AOE`
- `number` : le numéro à vérifier

Utilisez les fonctions d'aide définies dans `legal_numbers.h` pour ajouter ou supprimer des numéros depuis votre application. Vous pouvez également insérer directement dans la base avec une requête SQL :

```sql
INSERT INTO cdc_aoe_numbers(id,username,elevage_id,type,number) VALUES(1,'admin',0,'CDC','CDC12345');
```

La liste est chargée à l'initialisation via `legal_numbers_init()`.
