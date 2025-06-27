idf_component_register(SRCS "storage.c"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls db legal esp_http_client fatfs sdmmc)
