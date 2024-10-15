# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(game VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags (add more flags as needed)
add_compile_options(-Wall -Wextra)

# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add GTK using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Add GTK3 include and link directories
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add SFML (adjust components as needed)
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# Add NFD Library (ensure libnfd.a is in the lib directory)
add_library(nfd STATIC IMPORTED)
set_target_properties(nfd PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libnfd.a)

# Set source and output directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)
set(LEVELS_DIR ${CMAKE_SOURCE_DIR}/levels)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

# Gather source files recursively
file(GLOB_RECURSE SRCS ${SRC_DIR}/*.cpp)

# Create the executable
add_executable(${PROJECT_NAME} ${SRCS})

# Link SFML, NFD, and GTK3 libraries
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics nfd ${GTK3_LIBRARIES})

# Ensure GTK3 linking flags are correctly applied (provided by pkg-config)
target_link_options(${PROJECT_NAME} PRIVATE ${GTK3_LDFLAGS})
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS_OTHER})

# macOS specific settings
if(APPLE)
    # Link macOS system frameworks required for SFML
    target_link_libraries(${PROJECT_NAME} 
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
    
    # Set up paths for SFML and GTK3 if installed via Homebrew
    # This assumes you have installed SFML and GTK3 via Homebrew
    find_package(SFML 2.5 REQUIRED system window graphics)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    # Add necessary flags for macOS if needed
    target_compile_options(${PROJECT_NAME} PRIVATE -D_MACOS)
endif()

