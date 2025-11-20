##############################################################################

cmake_minimum_required(VERSION 3.12)

project(rigid_bodies)
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 2)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION 1.2.0)


##############################################################################
# paths
##############################################################################

set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})


##############################################################################
# dependencies
##############################################################################

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)


##############################################################################
# compiler flags
##############################################################################

# we need C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows needs special treatment
if(WIN32)
  add_compile_definitions(_USE_MATH_DEFINES NOMINMAX _CRT_SECURE_NO_WARNINGS)
  add_compile_options(/wd4267) # conversion from size_t
  add_compile_options(/wd4244) # conversion from double to float
  add_compile_options(/wd4305) # truncation from double to float
  add_compile_options(/wd4005) # macro redefinition
endif()

# emscripten is used to compile web-demos
if (EMSCRIPTEN)
  add_compile_options(--no-heap-copy)
  add_link_options("SHELL:-s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s MINIFY_HTML=0")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()


##############################################################################
# build type
##############################################################################

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()



##############################################################################
# STB 
# (place *before* GLFW since GLFW has an old copy of stb_image_write.h)
##############################################################################

set(STB_SOURCE_DIR "external/stb_image")
include_directories(${STB_SOURCE_DIR})
add_subdirectory(${STB_SOURCE_DIR})


##############################################################################
# GLFW
##############################################################################

if (NOT EMSCRIPTEN)
  set(GLFW_SOURCE_DIR  "external/glfw")
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
  set(GLFW_INSTALL OFF CACHE BOOL "")
  add_subdirectory(${GLFW_SOURCE_DIR})
  include_directories(${GLFW_SOURCE_DIR}/include
                      ${GLFW_SOURCE_DIR}/deps)
endif()


##############################################################################
# GLEW
##############################################################################

if (NOT EMSCRIPTEN)
  set(GLEW_SOURCE_DIR  "external/glew")
  include_directories(${GLEW_SOURCE_DIR}/include)
  add_definitions(-DGLEW_STATIC)
  add_library(glew STATIC
              ${GLEW_SOURCE_DIR}/src/glew.c
              ${GLEW_SOURCE_DIR}/include)
  target_link_libraries(glew ${GLFW_LIBRARIES})
endif()


##############################################################################
# imgui
##############################################################################

set(IMGUI_SOURCE_DIR "external/imgui")
include_directories(${IMGUI_SOURCE_DIR})
add_subdirectory(${IMGUI_SOURCE_DIR})


##############################################################################
# pmp
##############################################################################

set(PMP_SOURCE_DIR "external/pmp")
include_directories(external)
add_subdirectory(external/pmp)


##############################################################################
# source code directories
##############################################################################

include(AddFileDependencies)
include_directories(${PROJECT_SOURCE_DIR}/src/)
add_subdirectory(src)


##############################################################################
