pre_project(STATIC)

add_project()

if(MSVC)
	#add_definitions(-DCURL_STATICLIB)
	
	#target_link_libraries(${TARGET_NAME})
	
else()
	target_link_libraries(${TARGET_NAME}
		
	)
endif()

post_project(LIB)

add_dependencies (${TARGET_NAME} base_lib)









