
include(CMakeParseArguments)

macro(combine_arguments _variable)
	set(_result "")
	foreach(_element ${${_variable}})
		message(">>> _element_single = ${_element} ")
		set(_result "${_result} \"${_element}\"")
	endforeach()
	string(STRIP "${_result}" _result)
	set(${_variable} "${_result}")
	message(">>>combine_arguments_klx:" ${_variable})
endmacro()

function(export_all_flags _filename)
	set(_include_directories "$<TARGET_PROPERTY:${_target},INCLUDE_DIRECTORIES>")
	set(_compile_definitions "$<TARGET_PROPERTY:${_target},COMPILE_DEFINITIONS>")
	
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
		set(_compile_flags "$<TARGET_PROPERTY:${_target},COMPILE_FLAGS> -O2 -fpIC -g -mavx2 -std=c++0x -DNDEBUG")
	else()
		set(_compile_flags "$<TARGET_PROPERTY:${_target},COMPILE_FLAGS> -O2 -fpIC -g -msse4.2 -std=c++0x -DNDEBUG")
	endif()
	set(_compile_options "$<TARGET_PROPERTY:${_target},COMPILE_OPTIONS>")
	set(_include_directories "$<$<BOOL:${_include_directories}>:-I$<JOIN:${_include_directories},\n-I>\n>")
	set(_compile_definitions "$<$<BOOL:${_compile_definitions}>:-D$<JOIN:${_compile_definitions},\n-D>\n>")
	set(_compile_flags "$<$<BOOL:${_compile_flags}>:$<JOIN:${_compile_flags},\n>\n>")
	set(_compile_options "$<$<BOOL:${_compile_options}>:$<JOIN:${_compile_options},\n>\n>")
	file(GENERATE OUTPUT "${_filename}" CONTENT "${_compile_definitions}${_include_directories}${_compile_flags}${_compile_options}\n")
endfunction()

function(add_precompiled_header _target _input)
	cmake_parse_arguments(_PCH "FORCEINCLUDE" "SOURCE_CXX;SOURCE_C" "" ${ARGN})
	
	get_filename_component(_input_we ${_input} NAME_WE)
	#message(">>>_input_we:" ${_input_we}) #_input_we : stdafx
	#message(">>>_target:" ${_target})     #_target : scene_server
	if(NOT _PCH_SOURCE_CXX)
		#message(">>>_PCH_SOURCE_CXX")
		set(_PCH_SOURCE_CXX "${_input_we}.cpp")
	endif()
	if(NOT _PCH_SOURCE_C)
		#message(">>>_PCH_SOURCE_C")       #_PCH_SOURCE_C
		set(_PCH_SOURCE_C "${_input_we}.c")
	endif()
	
	if(MSVC)
		set(_cxx_path "${CMAKE_CURRENT_BINARY_DIR}/${_target}_cxx_pch")
		set(_c_path "${CMAKE_CURRENT_BINARY_DIR}/${_target}_c_pch")
		file(MAKE_DIRECTORY "${_cxx_path}")
		file(MAKE_DIRECTORY "${_c_path}")
		set(_pch_cxx_header "${_cxx_path}/${_input}")
		set(_pch_cxx_pch    "${_cxx_path}/${_input_we}.pch" )
		set(_pch_c_header   "${_c_path}/${_input}")
		set(_pch_c_pch      "${_c_path}/${_input_we}.pch")
		
		#message("pch_cxx_header:" ${_pch_cxx_header})    #stdafx.h
		#message("pch_cxx_pch:" ${_pch_cxx_pch})		  #cxx_Pch目录下的stdafx.pch
		#message("pch_c_header:" ${_pch_c_header})	      #c_pch下的stdafx.h	
		#message("pch_c_pch:" ${_pch_c_pch})			  #c_pch目录下的stdafx.pch
		#message("_cxx_path:" ${_cxx_path})               #cxx_pch目录
		#message("_c_path:" ${_c_path})    	              #c_pch目录			
		
		get_target_property(sources ${_target} SOURCES)
		foreach(_source ${sources})
			message(">>>_source:" ${_source})
			set(_pch_compile_flags "")
			if(_source MATCHES \\.\(cc|cxx|cpp|c\)$)
				if(_source MATCHES \\.\(cpp|cxx|cc\)$)
					set(_pch_header "${_input}")
					#message("_pch_header:" ${_pch_header}) #stdafx.h
					set(_pch "${_pch_cxx_pch}")
					#message("_pch:" ${_pch})               #cxx_pch目录下的stdafx.pch
				else()
					set(_pch_header "${_input}")
					message("_pch_header_2:" ${_pch_header})
					set(_pch "${_pch_c_pch}")
					message("_pch_2:" ${_pch})
				endif()
				
				if(_source MATCHES "${_PCH_SOURCE_CXX}") 
					#message("_PCH_SOURCE_CXX:" ${_PCH_SOURCE_CXX}) #_PCH_SOURCE_CXX:stdafx.cpp
					set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" \"/Yc${_input}\"")
					message("compile_flags_cxx:"  ${_pch_compile_flags})
					#cxx_pch下的
					set(_pch_source_cxx_found TRUE)
					set_source_files_properties("${_source}" PROPERTIES OBJECT_OUTPUTS "${_pch_cxx_pch}")
					message(">klx:_pch_cxx_pch:" ${_pch_cxx_pch})
					message(">klx:_source:" ${_source})
				elseif(_source MATCHES "${_PCH_SOURCE_C}")
					set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" \"/Yc${_input}\"")
					message("compile_flags_c:"  ${_pch_compile_flags})
					set(_pch_source_c_found TRUE)
					set_source_files_properties("${_source}" PROPERTIES OBJECT_OUTPUTS "${_pch_c_pch}")
					message(">>klx:_source:" ${_source})
				else()
					if(_source MATCHES \\.\(cpp|cxx|cc\)$)  #scene_server.cpp
						set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" \"/Yu${_input}\"")
						message("compile_flags_cxx_pch:" ${_pch_compile_flags})
						set(_pch_source_cxx_needed TRUE)
						set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_cxx_pch}")
						message(">>>klx:_pch_cxx_pch:" ${_pch_cxx_pch}) #stdafx.pch
						message(">>>klx:_source:" ${_source})           #scene_server.cpp
					else()
						set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_c_pch}\" \"/Yu${_input}\"")
						message("compile_flags_c_pch:" ${_pch_compile_flags})
						set(_pch_source_c_needed TRUE)
						set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_c_pch}")
					endif()
					if(_PCH_FORCEINCLUDE)
						set(_pch_compile_flags "${_pch_compile_flags} /FI${_input}")
					endif(_PCH_FORCEINCLUDE)
				endif()
					
				get_source_file_property(_object_depends "${_source}"  OBJECT_DEPENDS)
				
				if(NOT _object_depends)
					set(_object_depends)
				endif()
				
				if(_PCH_FORCEINCLUDE)
					if(_source MATCHES \\.\(cc|cxx|cpp\)$)
						list(APPEND _object_depends "${_pch_header}")
					else()
						list(APPEND _object_depends "${_pch_header}")
					endif()
				endif(_PCH_FORCEINCLUDE) 
				
				message("_source_depents:" ${_source})
				message("_object_depends:" ${_object_depends})

				set_source_files_properties(${_source} PROPERTIES
					COMPILE_FLAGS "${_pch_compile_flags}"
					OBJECT_DEPENDS "${_object_depends}" ) #增加stdafx.h的依赖  或其他编译stdafx.pch
			endif()
		endforeach()
		
		if(_pch_source_cxx_needed AND NOT _pch_source_cxx_found)
			message(FATAL_ERROR "A source file ${_PCH_SOURCE_CXX} for ${_input} is requried for MSVC build. Can be set with the SOURCE_CXX option.")
		endif()
		
		if(_pch_source_c_needed AND NOT _pch_source_c_found)
			message(FATAL_ERROR "A source file ${_PCH_SOURCE_C} for ${_input} is requried for MSVC build. Can be set with the SOURCE_C option.")
		endif()
	endif(MSVC)
endfunction()
