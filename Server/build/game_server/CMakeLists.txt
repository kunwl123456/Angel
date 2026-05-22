pre_project(RUNTIME)

add_project()

if(MSVC)
	target_link_libraries(${TARGET_NAME}
		debug game_world_d.lib
		optimized game_world.lib
		debug game_data_d.lib
		optimized game_data.lib
	)
else()
	target_link_libraries(${TARGET_NAME}
		game_world
		game_data
	)
endif ()

post_project(SERVER)

add_dependencies (${TARGET_NAME} game_world game_data)

