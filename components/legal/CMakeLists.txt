idf_component_register(
    SRCS "legal.c" "legal_templates.c"
    INCLUDE_DIRS "."
    # Needed so that legal.c can include animals.h
    REQUIRES animals db ui
)
