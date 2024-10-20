cmake_minimum_required(VERSION 3.12)

include(CheckIPOSupported)
check_ipo_supported(RESULT _IsIPOSupported)
if(_IsIPOSupported)
	message(STATUS "Enabled IPO optimization")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	message(STATUS "Enabled ccache: ${CCACHE_FOUND}")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND}) # Less useful to do it for linking, see edit2
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

# set the project name
project(PAPP
	VERSION 0.8.3
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(EMBED_PY_SRC "Enabled Python Source Embeded or not." OFF)

if(MSVC)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_subdirectory(
	3rd/pocketpy
	EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} src/main.cc)
target_compile_definitions(pocketpy PUBLIC -DPK_ENABLE_OS=1 -DPK_ENABLE_THREAD=1)
target_link_libraries(${PROJECT_NAME} pocketpy)

# linux cmake cant recognize .rc file
target_sources(${PROJECT_NAME} PRIVATE src/main.rc)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wno-calloc-transposed-args) # fix gcc-14 error for zip
endif()

# msys2 mingw32 link shared stdc++ lib
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	# add_link_options(-static-libgcc -static-libstdc++ -static)
	target_link_libraries(${PROJECT_NAME} -static gcc stdc++ pthread -dynamic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_link_libraries(${PROJECT_NAME} -Wl,-allow-multiple-definition) # for force fix allow Clang and GCC ABI error (duplicate section has different size). See https://doc.magnum.graphics/magnum/platforms-windows.html#platform-windows-troubleshooting-duplicate-section-has-different-size
	target_link_libraries(${PROJECT_NAME} -static stdc++ pthread -dynamic)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Deb")
	set(EMBED_PY_SRC OFF)
else()
	target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
	# strip release
	target_link_libraries(${PROJECT_NAME} -s)
endif()

if(EMBED_PY_SRC)
	message(STATUS "Enabled Python Source Embeded.")
	target_compile_definitions(${PROJECT_NAME} PUBLIC -D__EMBED_PY_SRC)
	file(READ src/main.py __PY_SRC__)
else()
	message(STATUS "Disabled Python Source Embeded.")
endif()

configure_file(
	src/get_pysrc.h.in
	${PROJECT_SOURCE_DIR}/src/get_pysrc.h
)
