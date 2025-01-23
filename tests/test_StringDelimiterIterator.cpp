//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StringDelimiterIterator.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace ParserTools;

TEST_CASE("Comma-separated strings")
{
    std::string text = "Abc, def, ghi";
    StringDelimiterIterator iterator(text, FindSubstring(", "));
    REQUIRE(iterator.next());
    REQUIRE(iterator.string() == "Abc");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remainder() == "def, ghi");
    REQUIRE(iterator.next());
    REQUIRE(iterator.string() == "def");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remainder() == "ghi");
    REQUIRE(iterator.next());
    REQUIRE(iterator.string() == "ghi");
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remainder().empty());
    REQUIRE(!iterator.next());
    REQUIRE(iterator.string().empty());
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remainder().empty());
}

TEST_CASE("String ends on delimiter")
{
    std::string text = "Abc, def.";
    StringDelimiterIterator iterator(text, FindSequenceOf(" ,."));
    REQUIRE(iterator.next());
    REQUIRE(iterator.string() == "Abc");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remainder() == "def.");
    REQUIRE(iterator.next());
    REQUIRE(iterator.string() == "def");
    REQUIRE(iterator.delimiter() == ".");
    REQUIRE(iterator.remainder().empty());
    REQUIRE(!iterator.next());
}
