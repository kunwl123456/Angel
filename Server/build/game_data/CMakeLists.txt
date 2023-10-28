pre_project(STATIC)


if(MSVC)
	file(GLOB_RECURSE DEF_FILES
		"${BASE_DIR}/inc/define/*.h"
	)
	list (REMOVE_ITEM DEF_FILES "${BASE_DIR}/inc/define/tglog_print.h")
	set(SRC_FILES ${SRC_FILES} ${DEF_FILES})

endif()

process_proto("../Shared/proto")
process_proto("src/game_data/proto")

add_project()

post_project(LIB)


add_dependencies(${TARGET_NAME} base_lib)


if(UNIX)
	target_link_libraries(${TARGET_NAME} base_lib)
endif()

if(MSVC)
	file(GLOB_RECURSE DATA_FILES "${TARGET_DIR}/inc/game_data/*.h")
	foreach(DATA_FILE ${DATA_FILES})
		source_group("data" FILES ${DATA_FILE})
	endforeach()

endif()































