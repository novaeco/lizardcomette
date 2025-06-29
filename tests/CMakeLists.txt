idf_component_register(SRCS "test_auth.c" "test_db.c" "test_legal.c" "test_crud.c" \
                       "test_scheduler.c" "test_ui.c" "test_animals_validation.c"
                       INCLUDE_DIRS ".."
                     REQUIRES db sqlite3 auth legal legal_numbers animals terrariums stocks transactions scheduler storage unity)
