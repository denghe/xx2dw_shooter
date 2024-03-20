cmake_minimum_required(VERSION 3.10)

# Default to release build if not specified
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()


set(name tool_pics2loadcode)
project(${name})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_definitions(-DDISABLE_ENGINE_AUDIO)


include_directories(
	./
	../nameof/include
	../xxlib
)


file(GLOB sources
	../nameof/include/*.hpp
	../xxlib/*.cpp
	../xxlib/*.c
	../xxlib/*.hpp
	../xxlib/*.h
	*.cpp
	*.c
	*.h
)
add_executable(${name} ${sources})


source_group("" FILES ${sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../ FILES ${sources})



if(MSVC)

	target_link_options(${name} PUBLIC
		$<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>
		$<$<CONFIG:MinSizeRel>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:Release>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB:libcmt.lib>
	)
	
	target_compile_definitions(${name} PUBLIC
		"$<$<CONFIG:MinSizeRel>:_SECURE_SCL=0>"
		"$<$<CONFIG:MinSizeRel>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:Release>:_SECURE_SCL=0>"
		"$<$<CONFIG:Release>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:RelWithDebInfo>:_SECURE_SCL=0>"
		"$<$<CONFIG:RelWithDebInfo>:_HAS_ITERATOR_DEBUGGING=0>"
	)

	add_compile_options($<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
	target_compile_options(${name}
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>
	)

	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../map_td3/pics/")

	set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
	target_compile_options(${name} PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)
	
else()

	target_precompile_headers(${name}
		PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>
	)
	
	target_compile_options(${name} PRIVATE -Wno-invalid-offsetof)
	
endif()
