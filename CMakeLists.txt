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

# Add NFD Library (ensure libnfd.a is in the correct directory for each platform)
add_library(nfd STATIC IMPORTED)

# Set source and output directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)
set(LEVELS_DIR ${CMAKE_SOURCE_DIR}/levels)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

# Gather source files recursively
file(GLOB_RECURSE SRCS ${SRC_DIR}/*.cpp)

# Create the executable named "game"
add_executable(game ${SRCS})

# Platform-specific settings and linking
if(APPLE)
    # macOS-specific NFD library path
    set_target_properties(nfd PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libnfd.a)

    # Link macOS system frameworks required for SFML
    target_link_libraries(game 
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreFoundation"
    )

    # Add necessary flags for macOS if needed
    target_compile_options(game PRIVATE -D_MACOS)
else()
    # Non-macOS-specific NFD library path
    set_target_properties(nfd PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libnfd.a)
endif()

# Find X11 and Xtst libraries
find_package(X11 REQUIRED)
find_library(XTST_LIB Xtst REQUIRED)

# Link SFML, NFD, GTK3, X11, and Xtst libraries
target_link_libraries(game sfml-system sfml-window sfml-graphics nfd ${GTK3_LIBRARIES} ${X11_LIBRARIES} ${XTST_LIB})

# Ensure GTK3 linking flags are correctly applied (provided by pkg-config)
target_link_options(game PRIVATE ${GTK3_LDFLAGS})
target_compile_options(game PRIVATE ${GTK3_CFLAGS_OTHER})

