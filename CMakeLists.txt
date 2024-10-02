# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(game VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags (equivalent to CXXFLAGS in Makefile)
add_compile_options(-Wall -Wextra)

# Add directories where header files are located (equivalent to -Iinclude)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add GTK (using pkg-config to find GTK like in the Makefile)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add SFML (adjust components as needed)
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# Add NFD Library (assuming libnfd.a is in the lib directory)
add_library(nfd STATIC IMPORTED)
set_target_properties(nfd PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libnfd.a)
set_target_properties(nfd PROPERTIES LINKER_LANGUAGE C)

# Set source and output directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)
set(LEVELS_DIR ${CMAKE_SOURCE_DIR}/levels)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

# Gather source files (equivalent to SRCS in Makefile)
file(GLOB_RECURSE SRCS ${SRC_DIR}/*.cpp)

# Create an executable
add_executable(${PROJECT_NAME} ${SRCS})

# Link the SFML, NFD, and GTK libraries (equivalent to $(SFML_LIBS), $(NFD_LIB), and $(GTK_LIBS))
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics nfd ${GTK3_LIBRARIES})

