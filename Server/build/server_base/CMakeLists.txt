pre_project(STATIC)

add_project()

post_project(LIB)

add_dependencies(${TARGET_NAME} core_lib base_lib)