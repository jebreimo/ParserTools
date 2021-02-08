# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-22.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)
project(ParserTools VERSION 0.1.1)

# Set PARSERTOOLS_MASTER_PROJECT to ON if ParserTools is the root level project.
if (NOT DEFINED PARSERTOOLS_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(PARSERTOOLS_MASTER_PROJECT ON)
    else()
        set(PARSERTOOLS_MASTER_PROJECT OFF)
    endif()
endif ()

# Test option
option(PARSERTOOLS_BUILD_TEST "Build tests" ${PARSERTOOLS_MASTER_PROJECT})

# Install option
option(PARSERTOOLS_INSTALL "Generate the install target" ${PARSERTOOLS_MASTER_PROJECT})

set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

add_library(ParserTools INTERFACE)

target_include_directories(ParserTools
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

add_library(ParserTools::ParserTools ALIAS ParserTools)

if (PARSERTOOLS_BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

export(TARGETS ParserTools
    NAMESPACE ParserTools::
    FILE ParserToolsConfig.cmake)

if (PARSERTOOLS_INSTALL)
    install(
        DIRECTORY
            include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(ParserToolsConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ParserToolsConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ParserTools
        )
endif()
