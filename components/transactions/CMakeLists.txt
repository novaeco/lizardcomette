idf_component_register(SRCS "transactions.c"
                       INCLUDE_DIRS "."
                       REQUIRES db sqlite3 animals legal storage)
