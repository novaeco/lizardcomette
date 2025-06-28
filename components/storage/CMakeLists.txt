idf_component_register(SRCS "storage.c"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls db legal esp_http_client fatfs sdmmc)

# Expose project configuration options
set_property(TARGET ${COMPONENT_LIB} PROPERTY
             KCONFIG_PROJBUILD ${CMAKE_CURRENT_LIST_DIR}/Kconfig.projbuild)
