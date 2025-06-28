idf_component_register(SRCS "auth.c"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls db sqlite3)
