//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StringDelimiterIterator.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch.hpp>

using namespace ParserTools;

TEST_CASE("Comma-separated strings")
{
    std::string text = "Abc, def, ghi";
    StringDelimiterIterator iterator(text, FindSubstring(", "));
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "Abc");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remaining_substring() == "def, ghi");
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "def");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remaining_substring() == "ghi");
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "ghi");
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remaining_substring().empty());
    REQUIRE(!iterator.next());
    REQUIRE(iterator.preceding_substring().empty());
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remaining_substring().empty());
}

TEST_CASE("String ends on delimiter")
{
    std::string text = "Abc, def.";
    StringDelimiterIterator iterator(text, FindSequenceOf(" ,."));
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "Abc");
    REQUIRE(iterator.delimiter() == ", ");
    REQUIRE(iterator.remaining_substring() == "def.");
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "def");
    REQUIRE(iterator.delimiter() == ".");
    REQUIRE(iterator.remaining_substring().empty());
    REQUIRE(!iterator.next());
}
