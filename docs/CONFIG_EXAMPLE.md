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
CONFIG_CO2_SDA_GPIO=21
CONFIG_CO2_SCL_GPIO=22
CONFIG_CO2_I2C_ADDR=0x61
CONFIG_LIGHT_SENSOR_REST_URL="http://example.com/light_sensor"
CONFIG_LIGHT_SENSOR_MQTT_TOPIC="/lizard/lux"
CONFIG_LIGHT_SENSOR_ADDR=0x23
```

Ce fichier illustre comment définir certaines options via `sdkconfig`.
`CONFIG_STORAGE_TRANSFER_URL` indique l'endpoint HTTP pour l'envoi de fichiers.
L'authentification HTTP est facultative. Si elle est nécessaire,
renseignez `CONFIG_STORAGE_TRANSFER_USERNAME` et
`CONFIG_STORAGE_TRANSFER_PASSWORD` ; sinon, ces variables peuvent être
laissées vides ou omises.
Ces trois options sont définies dans `components/storage/Kconfig.projbuild` et
contrôlent le transfert de fichiers via le Wi‑Fi.
Lorsque ces paramètres sont renseignés, la commande `db_backup()` du firmware
crée une sauvegarde chiffrée puis la transmet automatiquement à l'URL fournie.

`CONFIG_LIGHTING_PWM_GPIO` indique le GPIO utilisé pour piloter l'éclairage.
Les options `CONFIG_CO2_SDA_GPIO`, `CONFIG_CO2_SCL_GPIO` et
`CONFIG_CO2_I2C_ADDR` permettent d'adapter le câblage du capteur CO2.
`CONFIG_LIGHT_SENSOR_REST_URL` et `CONFIG_LIGHT_SENSOR_MQTT_TOPIC` définissent
les destinations réseau pour la luminosité mesurée, tandis que
`CONFIG_LIGHT_SENSOR_ADDR` précise l'adresse I2C du capteur BH1750.

`CONFIG_DB_DEFAULT_KEY` permet de définir la clé SQLCipher utilisée lors du
premier démarrage. Laisser ce champ vide obligera l'utilisateur à saisir la
clé manuellement.

Pour r\xC3\xA9importer une sauvegarde CSV, copiez le fichier chiffr\xC3\xA9 sur la
carte SD puis utilisez le bouton **Import CSV** dans l'interface. La base sera
alors vid\xC3\xA9e et remplie avec les donn\xC3\xA9es du fichier.
