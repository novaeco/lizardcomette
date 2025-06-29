idf_component_register(SRCS "drivers.c" "lighting.c" "co2.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_http_client mqtt)

# Expose project configuration options
set_property(TARGET ${COMPONENT_LIB} PROPERTY
             KCONFIG_PROJBUILD ${CMAKE_CURRENT_LIST_DIR}/Kconfig.projbuild)
