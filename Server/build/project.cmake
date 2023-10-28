function (pre_project TARGET_TYPE)
	#message(">>>URRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
	message(">>>BASE_DIR" ${BASE_DIR})
	get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	#message(">>>after TARGET_NAME:" ${TARGET_NAME})
	get_filename_component(TARGET_DIR ${BASE_DIR}/src/${TARGET_NAME} ABSOLUTE)
	get_filename_component(TARGET_INC_DIR ${BASE_DIR}/inc/${TARGET_NAME} ABSOLUTE) 
	set(TARGET_NAME ${TARGET_NAME} PARENT_SCOPE)
	set(TARGET_DIR  ${TARGET_DIR}  PARENT_SCOPE)
	set(TARGET_INC_DIR ${TARGET_INC_DIR} PARENT_SCOPE)
	set(TARGET_TYPE ${TARGET_TYPE} PARENT_SCOPE)
	
	#message(">>>TARGET_DIR:" ${TARGET_DIR})
	#message(">>>TARGET_INC_DIR:" ${TARGET_INC_DIR})
	file(GLOB_RECURSE SRC_FILES
		"${TARGET_DIR}/*.cpp"
		"${TARGET_DIR}/*.c"
		"${TARGET_DIR}/*.cc"
		"${TARGET_DIR}/*.h"
		"${TARGET_INC_DIR}/*.h"
	)
	
	#if(EXISTS "${TARGET_INC_DIRDIR}")
	#	message(">>>path TARGET_INC_DIRDIR exist:")
	#	file(GLOB_RECURSE SRC_FILES ${TARGET_INC_DIRDIR}/*.h)
	#endif()
	
	#foreach(src_file ${SRC_FILES})
	#	message(">>>src_file:" ${src_file})
	#endforeach()
	
	set(SRC_FILES ${SRC_FILES} PARENT_SCOPE)
	
	message(--------------------------------------)
	message(pre_project\ ${TARGET_NAME})
endfunction()

function(add_project)
	if(MSVC)
		add_definitions("/D WIN32" "/D WIN64" "/D _WINDOWS")
	else()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			#add_definitions(-D_DEBUG)
		endif()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
		endif()
		add_definitions(-D_LINUX)
	endif()
	
	include_directories(${TARGET_DIR})
	
	if(MSVC)
		add_compile_options("/Yu stdafx.h")
		add_compile_options("/Zi")
	endif()
	
	if(TARGET_TYPE STREQUAL "STATIC")
		add_library(${TARGET_NAME} STATIC ${SRC_FILES})
	elseif(TARGET_TYPE STREQUAL "RUNTIME")
		add_executable(${TARGET_NAME} ${SRC_FILES})
	endif()
	
	message(\ add_project\ ${TARGET_NAME})
endfunction()

function (post_project APP_TYPE)
	set(APP_TYPE ${APP_TYPE} PARENT_SCOPE)
	
	message(">>>TARGET_NAME:" ${TARGET_NAME})
	add_precompiled_header (${TARGET_NAME} stdafx.h FORCEINCLUDE SOURCE_CXX stdafx.cpp)
	#
	if(TARGET_TYPE STREQUAL "STATIC")
		set_target_properties(${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
		set_target_properties(${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/lib)
		set_target_properties(${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/lib)
	elseif(TARGET_TYPE STREQUAL "RUNTIME")
		set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/bin/${TARGET_NAME})
		set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/bin/${TARGET_NAME})
	endif()
	
	set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME}_d)
	set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${TARGET_NAME})
	
	if(UNIX)
		target_link_directories(${TARGET_NAME}
			
		)
		if(APP_TYPE STREQUAL "SERVER")
			target_link_libraries(${TARGET_NAME}
				server_base
				core_lib
			)
		endif()
		if(TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries(${TARGET_NAME}
				base_lib
				expat
				pthread
			)
		endif()
	elseif(MSVC)
		target_link_libraries(${TARGET_NAME}
			
		)
		if(TARGET_TYPE STREQUAL "RUNTIME")
			target_link_libraries(${TARGET_NAME}
				debug base_lib_d.lib
				optimized base_lib.lib
			)
		endif()
		if(TARGET_TYPE STREQUAL "SERVER")
			target_link_libraries(${TARGET_NAME}
				debug libexpatd.lib
				optimized libexpat.lib
				debug server_base_d.lib
				optimized server_base.lib
				debug core_lib_d.lib
				optimized core_lib.lib
			)
		endif()
	endif()
	
	if(APP_TYPE STREQUAL "SERVER")
		add_dependencies(${TARGET_NAME} server_base core_lib base_lib)
	endif()
	
	if(MSVC)
		set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE /DEBUG)
		foreach(SRC_FILE ${SRC_FILES})
			set(PARENT_DIR "")
			
			if(${SRC_FILE} MATCHES ".*[.]h$")
				if(${SRC_FILE} MATCHES ".*_inl[.]h$")
					string(REGEX REPLACE "_inl[.]h$" ".c" CPP_FILE ${SRC_FILE})
				else()
					string(REGEX REPLACE "[.]h$" ".c" CPP_FILE ${SRC_FILE})
				endif()
				get_filename_component(CPP_FILE ${CPP_FILE} NAME)
				foreach(F ${SRC_FILES})
					if(${F} MATCHES "/${CPP_FILE}")
						get_filename_component(PARENT_DIR "${F}" PATH)
						get_filename_component(PARENT_DIR "${PARENT_DIR}" NAME)
						set(FOLLOW_CPP TRUE)
						break()
					endif()
				endforeach()
			endif()
			
			if(${SRC_FILE} MATCHES ".*_data[.]h$")
				set(PARENT_DIR "data")
			endif()
			
			if((PARENT_DIR STREQUAL "") AND (${SRC_FILE} MATCHES ".*tolua_.*[.]cpp$"))
				set(PARENT_DIR "generated")
			endif()
			
			if(PARENT_DIR STREQUAL "")
				message(">>>SRC_FILE_message:" ${SRC_FILE})
				get_filename_component(PARENT_DIR "${SRC_FILE}" PATH)
				message(">>>PARENT_DIR_path:" ${PARENT_DIR})
				get_filename_component(PARENT_DIR "${PARENT_DIR}" NAME)
				message(">>>PARENT_DIR_name:" ${PARENT_DIR})
			endif()
			
			#message(">>>PARENT_DIR:" ${PARENT_DIR})
			#message(">>>TARGET_NAME:" ${TARGET_NAME})
			
			if(${PARENT_DIR} STREQUAL ${TARGET_NAME})
				set(PARENT_DIR "main")
			endif()
			
			source_group("${PARENT_DIR}" FILES "${SRC_FILE}")
			
			get_filename_component(SRC_FILE_NAME ${SRC_FILE} NAME)
			message(\tadded\ ${SRC_FILE_NAME}\ into\ group\ ${PARENT_DIR})
		endforeach()
	endif()
	message(post_project\ ${TARGET_NAME})
endfunction()








































































