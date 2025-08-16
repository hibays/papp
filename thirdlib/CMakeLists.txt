## 1. pocketpy

set(PKPY_ROOT pocketpy-2.0.8)
add_subdirectory(${PKPY_ROOT})

add_library(pkpy INTERFACE)
target_include_directories(pkpy INTERFACE ${PKPY_ROOT}/include)
target_link_libraries(pkpy INTERFACE pocketpy)

add_library(lib::pkpy ALIAS pkpy)

## 2. detours

set(DETOURS_ROOT Detours-4.0.1)
set(DETOURS_SOURCE ${DETOURS_ROOT}/src)

add_library(detours INTERFACE)
target_sources(detours PRIVATE
#	${DETOURS_SOURCE}/creatwth.cpp
	${DETOURS_SOURCE}/detours.cpp
	${DETOURS_SOURCE}/detours.h
	${DETOURS_SOURCE}/detver.h
	${DETOURS_SOURCE}/disasm.cpp
	${DETOURS_SOURCE}/disolarm.cpp
	${DETOURS_SOURCE}/disolarm64.cpp
	${DETOURS_SOURCE}/disolia64.cpp
	${DETOURS_SOURCE}/disolx64.cpp
	${DETOURS_SOURCE}/disolx86.cpp
	${DETOURS_SOURCE}/image.cpp
	${DETOURS_SOURCE}/modules.cpp
	${DETOURS_SOURCE}/uimports.cpp)

# This file is included and not compiled on its own
set_property(
	SOURCE ${DETOURS_SOURCE}/uimports.cpp
	APPEND PROPERTY HEADER_FILE_ONLY true)

target_include_directories(detours INTERFACE ${DETOURS_SOURCE})

add_library(lib::detours ALIAS detours)
