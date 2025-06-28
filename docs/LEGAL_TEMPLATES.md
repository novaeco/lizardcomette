# Modèles officiels CERFA et CITES

Ce projet utilise des gabarits PDF sur lesquels sont superposées les informations
issues de la base de données. Les formulaires vierges ne sont pas fournis pour
raison de droits d'auteur. Vous pouvez les télécharger gratuitement depuis les
sites officiels suivants :

- [service-public.fr](https://www.service-public.fr/) pour les différents CERFA
  (ex. 14367 et 12447).
- [cites.org](https://cites.org/) pour les certificats CITES d'import/export.

Placez les PDFs originaux dans le répertoire `forms/` en conservant les noms
suivants :

- `cerfa_14367_base.pdf`
- `cerfa_12447_base.pdf`
- `cites_import_base.pdf`
- `cites_export_base.pdf`

Les exemples fournis dans ce dépôt sont de simples fichiers factices permettant
uniquement de tester la génération de documents. Pour adapter les superpositions
à votre version des formulaires, modifiez les chaînes définies dans
`components/legal/legal_templates.c` en ajustant le texte et les positions
si nécessaire.
