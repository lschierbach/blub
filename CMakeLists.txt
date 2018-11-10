#minimum 3.7 needs confirmation
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Archive directory")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Library directory")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Runtime directory")
mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(BLUB_VERSION_MAJOR 0)
set(BLUB_VERSION_MINOR 1)

project(blub CXX)

find_package(SDL2 REQUIRED)
find_package(SDL_GPU REQUIRED)
if(UNIX)
    find_package(RapidJSON REQUIRED)
else()
    set(RAPIDJSON_INCLUDE_DIR $ENV{RAPIDJSONINCLUDE} CACHE PATH "Where rapidjson includes directory lives")
endif()
set(LIBS ${LIBS} ${SDL2_LIBRARY} ${SDL_GPU_LIBRARY})

set(USE_MWINDOWS ${USE_MWINDOWS} CACHE BOOL "Use -mwindows to remove terminal on mingw")
if(NOT WIN32)
    mark_as_advanced(USE_MWINDOWS)
endif()
if(USE_MWINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(blub ${SOURCES})

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL_GPU_INCLUDE_DIR})
include_directories(${RAPIDJSON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(blub ${LIBS})