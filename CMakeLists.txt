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

# set the project name
project(PAPP
	VERSION 0.8.5
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(EMBED_PY_SRC "Enabled Python Source Embeded or not." OFF)

if(MSVC)
	add_compile_options(/utf-8 /experimental:c11atomics)
endif()

add_subdirectory(thirdlib)

add_executable(${PROJECT_NAME}
	src/main.cc
	src/utils.cc
	src/lunmod.cc
	src/path.cc)

target_link_libraries(${PROJECT_NAME} lib::pkpy)

# This option is used to enable custom icon for the executable.
# Set it to ON, and define __CUSTOM_ICON_PATH__ to cmake
option(CUSTOM_ICON OFF)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wno-calloc-transposed-args) # fix gcc-14 error for zip
endif()

# msys2 mingw32 link shared stdc++ lib
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# add_link_options(-static-libgcc -static-libstdc++ -static)
	target_link_libraries(${PROJECT_NAME} -static gcc stdc++ pthread -dynamic)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_link_libraries(${PROJECT_NAME} -Wl,-allow-multiple-definition) # for force fix allow Clang and GCC ABI error (duplicate section has different size). See https://doc.magnum.graphics/magnum/platforms-windows.html#platform-windows-troubleshooting-duplicate-section-has-different-size
	target_link_libraries(${PROJECT_NAME} -static stdc++ pthread -dynamic)

elseif(MSVC)
	set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Deb")
	set(EMBED_PY_SRC OFF)

elseif(MSVC)
	target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
	target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:mainCRTStartup)

else()
	target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
	target_link_libraries(${PROJECT_NAME} -s) # strip release binary
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
	@ONLY
)

if(CUSTOM_ICON)
	configure_file(
		src/icon.rc.in
		${PROJECT_SOURCE_DIR}/src/icon.rc
		@ONLY
	)
	target_sources(${PROJECT_NAME} PRIVATE src/icon.rc)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_FILE:${PROJECT_NAME}>
	${CMAKE_CURRENT_SOURCE_DIR}/)

# 设置MSVC编译器的特殊处理
if(MSVC)
	# 手动适配clangd提示，不保真... 不知道会不会有什么奇怪的问题，出问题可以翻找一下cmake的缓存看看里面存了什么
	if(CMAKE_C_COMPILER MATCHES "cl.exe")
		get_filename_component(VC_INCLUDE_DIR ${CMAKE_C_COMPILER} DIRECTORY)
		get_filename_component(VC_INCLUDE_DIR ${VC_INCLUDE_DIR} DIRECTORY)
		get_filename_component(VC_INCLUDE_DIR ${VC_INCLUDE_DIR} DIRECTORY)
		get_filename_component(VC_INCLUDE_DIR ${VC_INCLUDE_DIR} DIRECTORY)

		set(DIASDK_DIR "${VC_INCLUDE_DIR}")
		get_filename_component(DIASDK_DIR ${DIASDK_DIR} DIRECTORY)
		get_filename_component(DIASDK_DIR ${DIASDK_DIR} DIRECTORY)
		get_filename_component(DIASDK_DIR ${DIASDK_DIR} DIRECTORY)
		get_filename_component(DIASDK_DIR ${DIASDK_DIR} DIRECTORY)
		set(DIASDK_DIR "${DIASDK_DIR}/DIA SDK/include")
		message(STATUS "DIASDK_DIR: ${DIASDK_DIR}")

		set(VC_INCLUDE_DIR "${VC_INCLUDE_DIR}/include")
		message(STATUS "VC_INCLUDE_DIR: ${VC_INCLUDE_DIR}")

		include_directories(
			BEFORE SYSTEM
			"${DIASDK_DIR}"
			"${VC_INCLUDE_DIR}"
		)
	endif()

	if(DEFINED CMAKE_MT)
		get_filename_component(WINDOWS_KITS_10_DIR ${CMAKE_MT} DIRECTORY)
		get_filename_component(_PLATFORM ${WINDOWS_KITS_10_DIR} NAME)
		get_filename_component(WINDOWS_KITS_10_DIR ${WINDOWS_KITS_10_DIR} DIRECTORY)
		get_filename_component(_VERSION ${WINDOWS_KITS_10_DIR} NAME)
		get_filename_component(WINDOWS_KITS_10_DIR ${WINDOWS_KITS_10_DIR} DIRECTORY)
		get_filename_component(WINDOWS_KITS_10_DIR ${WINDOWS_KITS_10_DIR} DIRECTORY)

		message(STATUS "WINDOWS_KITS_10_DIR: ${WINDOWS_KITS_10_DIR}")
		include_directories(
			BEFORE SYSTEM
			"${WINDOWS_KITS_10_DIR}/include/${_VERSION}/um"
			"${WINDOWS_KITS_10_DIR}/include/${_VERSION}/ucrt"
			"${WINDOWS_KITS_10_DIR}/include/${_VERSION}/shared"
			"${WINDOWS_KITS_10_DIR}/include/${_VERSION}/winrt"
		)
	endif()
endif()
