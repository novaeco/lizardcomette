idf_component_register(SRCS "test_auth.c" "test_db.c" "test_legal.c" "test_crud.c"
                       INCLUDE_DIRS ".."
                       REQUIRES db auth legal animals terrariums stocks transactions unity)
