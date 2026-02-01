//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StringTokenizer.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch_test_macros.hpp>

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
    REQUIRE(std::distance(tokenizer.begin(), tokenizer.end()) == 1);
}

TEST_CASE("Non-empty string without tokens")
{
    StringTokenizer tokenizer("foo", FindNewline());
    REQUIRE(std::distance(tokenizer.begin(), tokenizer.end()) == 1);
}

TEST_CASE("Test split")
{
    auto parts = split("abc\ndef\r\nghijkl\rmnopq\n\r", FindNewline());
    REQUIRE(parts.size() == 6);
    REQUIRE(parts[0] == "abc");
    REQUIRE(parts[1] == "def");
    REQUIRE(parts[2] == "ghijkl");
    REQUIRE(parts[3] == "mnopq");
    REQUIRE(parts[4].empty());
    REQUIRE(parts[5].empty());
}

TEST_CASE("Split with non-empty end.")
{
    auto parts = split("abc, def", FindSequenceOf(" ,"));
    REQUIRE(parts.size() == 2);
    REQUIRE(parts[0] == "abc");
    REQUIRE(parts[1] == "def");
}

TEST_CASE("Split empty string.")
{
    auto parts = split("", FindChar(','));
    REQUIRE(parts.size() == 1);
    REQUIRE(parts[0] == "");
}
