//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/StreamDelimiterIterator.hpp"
#include "ParserTools/DelimiterFinders.hpp"
#include <catch2/catch.hpp>

#include <sstream>

using namespace ParserTools;

TEST_CASE("False end of sequence at end of buffer_")
{
    std::string str = "ABCDEFGHIJ . . .BCDEFGHIJK";
    std::stringstream ss(str);
    StreamDelimiterIterator iterator(ss, FindSequenceOf(" ."), 12);
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "ABCDEFGHIJ");
    REQUIRE(iterator.delimiter() == " . . .");
    REQUIRE(iterator.remaining_buffer() == "BCDEFGHI");
    REQUIRE(iterator.next());
    REQUIRE(iterator.preceding_substring() == "BCDEFGHIJK");
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remaining_buffer().empty());
    REQUIRE(!iterator.next());
    REQUIRE(iterator.preceding_substring().empty());
    REQUIRE(iterator.delimiter().empty());
    REQUIRE(iterator.remaining_buffer().empty());
}
