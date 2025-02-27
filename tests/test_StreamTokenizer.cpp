//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StreamTokenizer.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace ParserTools;

TEST_CASE("Tokenize a stream")
{
    std::string str = "ABCDEFGHIJ . . .BCDEFGHIJK . ";
    std::stringstream ss(str);
    int i = 0;
    for (auto item : tokenize(ss, FindSequenceOf(" .")))
    {
        switch (i)
        {
        case 0:
            REQUIRE(item.string() == "ABCDEFGHIJ");
            break;
        case 1:
            REQUIRE(item.string() == "BCDEFGHIJK");
            break;
        case 2:
            REQUIRE(item.string().empty());
            break;
        default:
            FAIL("Too many tokens!");
        }
        ++i;
    }
}

TEST_CASE("Tokenize an empty stream")
{
    std::string str = "";
    std::stringstream ss(str);
    int i = 0;
    for (auto item : tokenize(ss, FindSequenceOf(" .")))
    {
        ++i;
    }
    REQUIRE(i == 1);
}
