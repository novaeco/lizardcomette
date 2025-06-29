idf_component_register(SRCS "db.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES sqlite3)

# Expose project configuration options
set_property(TARGET ${COMPONENT_LIB} PROPERTY
             KCONFIG_PROJBUILD ${CMAKE_CURRENT_LIST_DIR}/Kconfig.projbuild)
