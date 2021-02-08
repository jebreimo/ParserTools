# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-22.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v2.11.1"
    )
FetchContent_GetProperties(catch)
if (NOT catch_POPULATED)
    FetchContent_Populate(catch)
    add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif ()

add_executable(ParserToolsTest
    ParserToolsTest.cpp
    test_StreamDelimiterIterator.cpp
    test_StringDelimiterIterator.cpp
    test_StringTokenizer.cpp
    )

target_link_libraries(ParserToolsTest
    PRIVATE
        ParserTools::ParserTools
        Catch2::Catch2
    )
