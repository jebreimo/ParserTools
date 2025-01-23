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
    GIT_TAG "v3.8.0"
    )
FetchContent_MakeAvailable(catch)

add_executable(ParserToolsTest
    test_DelimiterFinders.cpp
    test_ParseDouble.cpp
    test_StreamDelimiterIterator.cpp
    test_StreamTokenizer.cpp
    test_StringDelimiterIterator.cpp
    test_StringTokenizer.cpp
    )

target_link_libraries(ParserToolsTest
    PRIVATE
        ParserTools::ParserTools
        Catch2::Catch2WithMain
    )
