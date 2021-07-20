# https://gitlab.com/CLIUtils/modern-cmake/-/blob/master/examples/extended-project/CMakeLists.txt

cmake_minimum_required(VERSION 3.19)

# Project name and a few useful settings. Other commands can pick up the results
project(
        libdc_application
        VERSION 0.1.0.0
        HOMEPAGE_URL "https://github.com/bcitcstdatacomm/libdc_application"
        DESCRIPTION ""
        LANGUAGES C)

set(HEADER_LIST
        "${libdc_application_SOURCE_DIR}/include/dc_application/application.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/settings.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/command_line.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/config.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/defaults.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/environment.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/options.h"
        )

set(SOURCE_LIST
        "${libdc_application_SOURCE_DIR}/src/application.c"
        "${libdc_application_SOURCE_DIR}/src/settings.c"
        "${libdc_application_SOURCE_DIR}/src/command_line.c"
        "${libdc_application_SOURCE_DIR}/src/config.c"
        "${libdc_application_SOURCE_DIR}/src/defaults.c"
        "${libdc_application_SOURCE_DIR}/src/environment.c"
        "${libdc_application_SOURCE_DIR}/src/options.c"
        )

### Require out-of-source builds
# this still creates a CMakeFiles directory and CMakeCache.txt- can we delete them?
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

    # Let's ensure -std=c11 instead of -std=g11
    set(CMAKE_C_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
# add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
        AND BUILD_TESTING)
#    add_subdirectory(tests)
endif()

