idf_component_register(SRCS "drivers.c" "lighting.c" "co2.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_http_client mqtt)
