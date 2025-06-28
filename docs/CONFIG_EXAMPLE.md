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
```

Ce fichier illustre comment définir certaines options via `sdkconfig`.
`CONFIG_STORAGE_TRANSFER_URL` indique l'endpoint HTTP pour l'envoi de fichiers.
Si le serveur requiert une authentification basique, renseignez
`CONFIG_STORAGE_TRANSFER_USERNAME` et `CONFIG_STORAGE_TRANSFER_PASSWORD`.
