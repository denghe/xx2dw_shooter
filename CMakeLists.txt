cmake_minimum_required(VERSION 3.10)

if(MSVC)
	# cmake when compiler choose: -T ClangCL
	set(CMAKE_GENERATOR_TOOLSET "ClangCL")
endif()

project(xx2dw_shooter)

add_subdirectory(shooter1)
add_subdirectory(shooter2)
