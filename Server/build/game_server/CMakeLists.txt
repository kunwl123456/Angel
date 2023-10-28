pre_project(RUNTIME)

add_project()

if(MSVC)
	target_link_libraries(${TARGET_NAME}
		debug game_world_d.lib
	)
else()
	target_link_libraries(${TARGET_NAME}
		game_world
	)
endif ()

post_project(SERVER)

add_dependencies (${TARGET_NAME} game_world)

