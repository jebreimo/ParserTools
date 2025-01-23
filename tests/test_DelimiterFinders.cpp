//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace ParserTools;

TEST_CASE("Test FindNewline")
{
    using P = std::pair<size_t, size_t>;

    REQUIRE(FindNewline()("\r\n") == P(0, 2));
    REQUIRE(FindNewline()("\r") == P(0, 1));
    REQUIRE(FindNewline()("\n") == P(0, 1));
    REQUIRE(FindNewline()("0123456\n89ABCDEF") == P(7, 8));
    REQUIRE(FindNewline()("\r ") == P(0, 1));
}
