//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StringTokenizer.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch.hpp>

using namespace ParserTools;

TEST_CASE("StringTokenizer on comma-separated strings")
{
    std::string text = "Abc, def, ghi";
    int i = 0;
    for (auto token : tokenize(text, FindSubstring(", ")))
    {
        switch (i)
        {
        case 0:
            REQUIRE(token.string() == "Abc");
            break;
        case 1:
            REQUIRE(token.string() == "def");
            break;
        case 2:
            REQUIRE(token.string() == "ghi");
            break;
        default:
            FAIL("Too many tokens!");
        }
        ++i;
    }
}

TEST_CASE("string is empty")
{
    StringTokenizer tokenizer({}, FindNewline());
    REQUIRE(std::distance(tokenizer.begin(), tokenizer.end()) == 0);
}

TEST_CASE("Non-empty string without tokens")
{
    StringTokenizer tokenizer("foo", FindNewline());
    REQUIRE(std::distance(tokenizer.begin(), tokenizer.end()) == 1);
}