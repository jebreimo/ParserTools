# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-22.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)
project(ParserTools)

set(CMAKE_CXX_STANDARD 17)

add_library(ParserTools INTERFACE)

target_include_directories(ParserTools
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

add_library(ParserTools::ParserTools ALIAS ParserTools)

enable_testing()
add_subdirectory(tests)
