pre_project(RUNTIME)

if(MSVC)
	file (GLOB_RECURSE MISC_FILES
		"${BASE_DIR}/inc/message/*.h"
		"${BASE_DIR}/bin/server_config/*.lua"
		"${BASE_DIR}/../Shared/config/*.lua"
		"${BASE_DIR}/../Shared/script/server/*.lua"
		"${BASE_DIR}/src/misc/script/*.lua"
		#"${BASE_DIR}/bin/misc/tglog.xml"
	)
	
	set(SRC_FILES ${SRC_FILES} ${MISC_FILES})
	
	list(APPEND SRC_FILES
		"${BASE_DIR}/build/CMakeLists.txt"
		"${BASE_DIR}/build/precompile.cmake"
		"${BASE_DIR}/build/project.cmake"
		"${BASE_DIR}/build/proto.cmake"
		#"${BASE_DIR}/build/qt.cmake"
	)
	
	#list(APPEND SRC_FILES
	#	"${BASE_DIR}/autobuild/template/idc/server_config/patch_1.lua"
	#	"${BASE_DIR}/autobuild/template/idc/server_config/patch_3.lua.template"
	#)
	
endif()


add_project()

post_project(MISC)

add_dependencies(${TARGET_NAME} base_lib)

foreach (SRC_FILE ${SRC_FILES})
	set(PARENT_DIR "")
	
	if(${SRC_FILE} MATCHES ".*[.]lua$")
		get_filename_component(PARENT_DIR "${SRC_FILE}" PATH)
		get_filename_component(PARENT_DIR "${PARENT_DIR}" NAME)
		set(PARENT_DIR "lua\\${PARENT_DIR}")
	endif()
	
	if(PARENT_DIR STREQUAL "")
		source_group("${PARENT_DIR}"  FILES "${SRC_FILE}")
	endif()
	
endforeach()










