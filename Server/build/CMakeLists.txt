cmake_minimum_required(VERSION 3.15)

get_filename_component(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
get_filename_component(FOLDER_NAME ${CMAKE_BINARY_DIR} NAME)

project(${FOLDER_NAME})

set(CMKA_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include (precompile.cmake)
include (project.cmake)
include (proto.cmake)

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif ()

if(_FINAL)
	add_definitions(-D_FINAL)
endif()

if(UNIX)
	message("os is linux")
	message("gcc version: ${CMAKE_CXX_COMPILER_VERSION}")
	
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release)
	endif()
	
	string(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Release" is_release)
	string(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Debug"  is_debug)
	
	if(${is_release})
		if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
			message("compile with avx2intrin")
			set(CMAKE_CXX_FLAGS "-fPIC -O2 -g -mavx2 -std=c++0x -Wpointer-arith -Warray-bounds")
			add_definitions(-DUSING_AVX)
		else()
			set(CMAKE_CXX_FLAGS "-fPIC -O2 -g -msse4.2 -std=c++0x -Wpointer-arith -Warray-bounds")
		endif()
	elseif(${is_debug})
		add_definitions(-DNDEBUG)
		if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
			set(CMAKE_CXX_FLAGS "-fPIC -g -mavx2 -std=c++11 -Wpointer-arith -Warray-bounds -fdiagnostics-color=auto")
			add_definitions(-DUSING_AVX)
		else()
			set(CMAKE_CXX_FLAGS "-fPIC -g -msse4.2 -std=c++1 -Wpointer-arith -Warray-bounds")
		endif()
	endif()
	
	message("flgs: ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
	message("os is windows")
	add_definitions(/MP)
	
	foreach (flag
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_DEBUG_INIT
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_DEBUG_INIT
		CMAKE_CXX_FLAGS
	)
		string (FIND ${flag} "RELEASE" is_release)
		string (FIND ${flag} "DEBUG"   is_debug)
		if(${is_release} GREATER -1)
			string (FIND "${${flag}}" "/MD" has_md)
			if(has_md EQUAL -1)
				set("${flag}" "${${flag}} /MD")
			endif()
		elseif(${is_debug} GREATER -1)
			string (FIND "${${flag}}" "/MDd" has_mdt)
			if(has_mdt EQUAL -1)
				set("${flag}" "${${flag}} /MDd")
			endif()
		endif()
		string (REPLACE "/MD" "/MT" "${flag}" "${${flag}}")
		string (REPLACE "/EHsc" "" "${flag}" "${${flag}}")
		set("${flag}" "${${flag}} /bigobj /GR- /fp:fast /Zc:wchar_t-")
		message("${flag} ${${flag}}")
	endforeach()
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /arch:AVX2 /Oi /Ot /GL")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /arch:AVX2 /Oi /Ot /GL")
endif()
	
include_directories(
	${BASE_DIR}/inc
	${BASE_DIR}/3rd
	${BASE_DIR}/inc/base_lib
	${BASE_DIR}/3rd/libevent/include
	${BASE_DIR}/3rd/tinyxml/include
)

get_filename_component(NFSHMXFRAME_ROOT ${BASE_DIR}/../../NFShmXFrame ABSOLUTE)
set(NFSHMXFRAME_LUABIND_DIR ${NFSHMXFRAME_ROOT}/thirdparty/LuaBind)
if(EXISTS ${NFSHMXFRAME_LUABIND_DIR}/luaintf/LuaIntf.h)
	message("NFShmXFrame LuaBind found: ${NFSHMXFRAME_LUABIND_DIR}")
	add_definitions(-DANGEL_WITH_NFSHMXFRAME_LUABIND)
	include_directories(
		${NFSHMXFRAME_LUABIND_DIR}
		${NFSHMXFRAME_LUABIND_DIR}/lua
		${NFSHMXFRAME_LUABIND_DIR}/luaintf
		${NFSHMXFRAME_LUABIND_DIR}/lpeg
		${NFSHMXFRAME_LUABIND_DIR}/luacjson
		${NFSHMXFRAME_LUABIND_DIR}/luafilesystem
		${NFSHMXFRAME_LUABIND_DIR}/lua_protobuf
		${NFSHMXFRAME_LUABIND_DIR}/lua-fmt
		${NFSHMXFRAME_LUABIND_DIR}/protoc-gen-lua
	)
	file(GLOB_RECURSE ANGEL_LUABIND_SRC
		"${NFSHMXFRAME_LUABIND_DIR}/lua/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/lpeg/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/luacjson/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/luafilesystem/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/lua_protobuf/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/lua-fmt/*.c"
		"${NFSHMXFRAME_LUABIND_DIR}/protoc-gen-lua/*.c"
	)
	add_library(angel_luabind STATIC ${ANGEL_LUABIND_SRC})
	target_include_directories(angel_luabind PUBLIC
		${NFSHMXFRAME_LUABIND_DIR}
		${NFSHMXFRAME_LUABIND_DIR}/lua
		${NFSHMXFRAME_LUABIND_DIR}/luaintf
		${NFSHMXFRAME_LUABIND_DIR}/lpeg
		${NFSHMXFRAME_LUABIND_DIR}/luacjson
		${NFSHMXFRAME_LUABIND_DIR}/luafilesystem
		${NFSHMXFRAME_LUABIND_DIR}/lua_protobuf
		${NFSHMXFRAME_LUABIND_DIR}/lua-fmt
		${NFSHMXFRAME_LUABIND_DIR}/protoc-gen-lua
	)
else()
	message("NFShmXFrame LuaBind not found; Angel Lua module will build as a skeleton")
endif()

link_directories(
	${BASE_DIR}/lib
	${BASE_DIR}/3rd
	${BASE_DIR}/3rd/libevent/lib
	${BASE_DIR}/3rd/tinyxml/lib
)

file(GLOB ANGEL_TINYXML_SRC
	"${BASE_DIR}/3rd/tinyxml/src/tinystr.cpp"
	"${BASE_DIR}/3rd/tinyxml/src/tinyxml.cpp"
	"${BASE_DIR}/3rd/tinyxml/src/tinyxmlerror.cpp"
	"${BASE_DIR}/3rd/tinyxml/src/tinyxmlparser.cpp"
)
add_library(angel_tinyxml STATIC ${ANGEL_TINYXML_SRC})
target_include_directories(angel_tinyxml PUBLIC ${BASE_DIR}/3rd/tinyxml/include ${BASE_DIR}/3rd/tinyxml/src)

preprocess_proto("../Shared/proto")
preprocess_proto("src/game_data/proto")
preprocess_proto("src/game_world/proto")

if(FOLDER_NAME STREQUAL "angel")
	add_definitions("/D_ANGEL")
endif()

add_subdirectory(core_lib)
add_subdirectory(base_lib)
add_subdirectory(game_data)
add_subdirectory(misc)
add_subdirectory(game_world)
add_subdirectory(server_base)


if(FOLDER_NAME STREQUAL "angel")
	add_subdirectory(game_server)
	add_subdirectory(gateway_server)
	add_subdirectory(world_server)
	add_subdirectory(platform_server)
	add_subdirectory(team_server)
	add_subdirectory(cross_server)
	add_subdirectory(master_control_server)
	add_subdirectory(friend_server)
	add_subdirectory(guild_server)
	add_subdirectory(global_server)
	add_subdirectory(dbmanager_server)
	add_subdirectory(dbcache_server)
	add_subdirectory(scene_server)
endif()
	



