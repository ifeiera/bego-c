cmake_minimum_required(VERSION 3.10)
project(bego-c VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Build shared library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(BUILD_SHARED_LIBS ON)

# Set MSVC runtime library to static
if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define the library source files
set(BEGO_SOURCES
    src/errors.cpp
    src/settings.cpp
    src/dpi_awareness.cpp
    src/bego_win.cpp
    src/bego_keyboard.cpp
    src/bego_mouse.cpp
    src/input_helpers.cpp
    src/key_converter.cpp
)

# Create the library (now as shared library due to BUILD_SHARED_LIBS ON)
add_library(bego ${BEGO_SOURCES})

# Link Windows libraries
if(WIN32)
    target_link_libraries(bego Shcore.lib User32.lib)
endif()

# Create the example executable
add_executable(bego-example src/example.cpp)
target_link_libraries(bego-example bego)

# Create the autopress example executable
add_executable(bego-autopress src/example_autopress.cpp)
target_link_libraries(bego-autopress bego)

# Installation rules
install(TARGETS bego DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/bego) 