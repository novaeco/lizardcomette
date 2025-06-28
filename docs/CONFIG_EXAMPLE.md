# Exemple de configuration

```
CONFIG_WIFI_SSID="VotreSSID"
CONFIG_WIFI_PASSWORD="VotreMotDePasse"
CONFIG_STORAGE_MOUNT_POINT="/sdcard"
CONFIG_STORAGE_TRANSFER_URL="http://example.com/upload"
CONFIG_STORAGE_TRANSFER_USERNAME="user"
CONFIG_STORAGE_TRANSFER_PASSWORD="secret"
CONFIG_DRIVERS_REST_URL="http://example.com/sensors"
CONFIG_DRIVERS_MQTT_URI="mqtt://broker"
CONFIG_DRIVERS_MQTT_TOPIC="/lizard/data"
CONFIG_DB_DEFAULT_KEY="MonMotDePasseBD"
CONFIG_LIGHTING_PWM_GPIO=5
CONFIG_LIGHTING_REST_URL="http://example.com/light"
CONFIG_LIGHTING_MQTT_TOPIC="/lizard/light"
CONFIG_CO2_REST_URL="http://example.com/co2"
CONFIG_CO2_MQTT_TOPIC="/lizard/co2"
```

Ce fichier illustre comment définir certaines options via `sdkconfig`.
`CONFIG_STORAGE_TRANSFER_URL` indique l'endpoint HTTP pour l'envoi de fichiers.
L'authentification HTTP est facultative. Si elle est nécessaire,
renseignez `CONFIG_STORAGE_TRANSFER_USERNAME` et
`CONFIG_STORAGE_TRANSFER_PASSWORD` ; sinon, ces variables peuvent être
laissées vides ou omises.

`CONFIG_DB_DEFAULT_KEY` permet de définir la clé SQLCipher utilisée lors du
premier démarrage. Laisser ce champ vide obligera l'utilisateur à saisir la
clé manuellement.

Pour r\xC3\xA9importer une sauvegarde CSV, copiez le fichier chiffr\xC3\xA9 sur la
carte SD puis utilisez le bouton **Import CSV** dans l'interface. La base sera
alors vid\xC3\xA9e et remplie avec les donn\xC3\xA9es du fichier.
