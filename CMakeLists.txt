cmake_minimum_required(VERSION 3.10)

# for compile emscripten only

if(MSVC)
	# cmake when compiler choose: -T ClangCL
	set(CMAKE_GENERATOR_TOOLSET "ClangCL")	
endif()

project(xx2dw_shooter)

SET(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "USE_MSVC_RUNTIME_LIBRARY_DLL" FORCE)
SET(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "ZSTD_BUILD_PROGRAMS" FORCE)
SET(ZSTD_BUILD_SHARED OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_BUILD_TESTS OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "ZSTD_LEGACY_SUPPORT" FORCE)
SET(ZSTD_MULTITHREAD_SUPPORT_DEFAULT OFF CACHE BOOL "ZSTD_MULTITHREAD_SUPPORT_DEFAULT" FORCE)
SET(ZSTD_MULTITHREAD_SUPPORT OFF CACHE BOOL "ZSTD_MULTITHREAD_SUPPORT" FORCE)
add_subdirectory(zstd/build/cmake zstd)
include_directories(
	zstd/lib
)

add_subdirectory(s1)
add_subdirectory(s2)
add_subdirectory(s3)
add_subdirectory(s4)
add_subdirectory(s5)
add_subdirectory(s6)
add_subdirectory(s7)
add_subdirectory(s8)
add_subdirectory(s9)
add_subdirectory(s10)
add_subdirectory(s11)
add_subdirectory(s12)
add_subdirectory(s13)
add_subdirectory(s14)
add_subdirectory(s15)
add_subdirectory(s16)
add_subdirectory(s17)
add_subdirectory(s18)
add_subdirectory(s19)
add_subdirectory(s20)
add_subdirectory(s21)
add_subdirectory(s22)
add_subdirectory(s23)
add_subdirectory(s24)
add_subdirectory(s25)
