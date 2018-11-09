#minimum 3.7 needs confirmation
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(BLUB_VERSION_MAJOR 0)
set(BLUB_VERSION_MINOR 1)

project(blub)

find_package(SDL2 REQUIRED)
find_package(SDL_GPU REQUIRED)
find_package(RapidJSON REQUIRED)
message(STATUS "SDL2 Include: " ${SDL2_INCLUDE_DIR})
message(STATUS "SDL_GPU Include " ${SDL_GPU_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SDL2_LIBRARY} ${SDL_GPU_LIBRARY})


file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(blub ${SOURCES})

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL_GPU_INCLUDE_DIR})
include_directories(${RAPIDJSON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(blub ${LIBS})