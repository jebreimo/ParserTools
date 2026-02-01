//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/ParseFloatingPoint.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

TEST_CASE("parse_floating_point")
{
    auto v = ParserTools::parse_floating_point<double>("");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("0");
    REQUIRE(v);
    REQUIRE(*v == 0);
    v = ParserTools::parse_floating_point<double>("1.");
    REQUIRE(v);
    REQUIRE(*v == 1);

    v = ParserTools::parse_floating_point<double>("0 ");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>(" 0");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>(".");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>(".0");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("e");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("+");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("-");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("-.2");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("-e2");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1.2");
    REQUIRE(v);
    REQUIRE(*v == 1.2);
    v = ParserTools::parse_floating_point<double>("123456.123456");
    REQUIRE(v);
    REQUIRE(*v == 123456.123456);
    v = ParserTools::parse_floating_point<double>("-123456.123456");
    REQUIRE(v);
    REQUIRE(*v == -123456.123456);
    v = ParserTools::parse_floating_point<double>("+123456.123456");
    REQUIRE(v);
    REQUIRE(*v == 123456.123456);
    v = ParserTools::parse_floating_point<double>("0e0");
    REQUIRE(v);
    REQUIRE(*v == 0);
    v = ParserTools::parse_floating_point<double>("1E5");
    REQUIRE(v);
    REQUIRE(*v == 1e5);
    v = ParserTools::parse_floating_point<double>("1.E5");
    REQUIRE(v);
    REQUIRE(*v == 1e5);
    v = ParserTools::parse_floating_point<double>("-1.E5");
    REQUIRE(v);
    REQUIRE(*v == -1e5);
    v = ParserTools::parse_floating_point<double>("1.2E");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1.2E-");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1.2E ");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1.2E2.");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1.2E2 ");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1234.5678e222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(1234.5678e222, 1e215));
    v = ParserTools::parse_floating_point<double>("1234.5678e-222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(1234.5678e-222, 1e-215));
    v = ParserTools::parse_floating_point<double>("1234.5678e+222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(1234.5678e222, 1e215));
    v = ParserTools::parse_floating_point<double>("-1234.5678e222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(-1234.5678e222, 1e215));
    v = ParserTools::parse_floating_point<double>("-1234.5678e-222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(-1234.5678e-222, 1e-225));
    v = ParserTools::parse_floating_point<double>("-1234.5678e+222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(-1234.5678e222, 1e215));
    v = ParserTools::parse_floating_point<double>("-1234.5678e+222");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(-1234.5678e222, 1e215));
    v = ParserTools::parse_floating_point<double>("2.2250738585072014e-308");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(2.2250738585072014e-308, 1e-318));
    v = ParserTools::parse_floating_point<double>("2.2250739e-308");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(2.2250739e-308, 1e-318));
    v = ParserTools::parse_floating_point<double>("1.7976931348623157e+308");
    REQUIRE(v);
    REQUIRE(*v == 1.7976931348623157e+308);
    v = ParserTools::parse_floating_point<double>("1e308");
    REQUIRE(v);
    REQUIRE_THAT(*v, WithinAbs(1e308, 1e298));
    v = ParserTools::parse_floating_point<double>("1e309");
    REQUIRE(!v);
    v = ParserTools::parse_floating_point<double>("1e-307");
    REQUIRE(v);
    REQUIRE(*v == 1e-307);
    v = ParserTools::parse_floating_point<double>("1e-308");
    REQUIRE(v);
    REQUIRE(*v == 1e-308);
    v = ParserTools::parse_floating_point<double>("NaN");
    REQUIRE(v);
    REQUIRE(std::isnan(*v));
    v = ParserTools::parse_floating_point<double>("null");
    REQUIRE(v);
    REQUIRE(std::isinf(*v));
    REQUIRE(*v > 0);
    v = ParserTools::parse_floating_point<double>("Infinity");
    REQUIRE(v);
    REQUIRE(std::isinf(*v));
    REQUIRE(*v > 0);
    v = ParserTools::parse_floating_point<double>("+Infinity");
    REQUIRE(v);
    REQUIRE(std::isinf(*v));
    REQUIRE(*v > 0);
    v = ParserTools::parse_floating_point<double>("-Infinity");
    REQUIRE(v);
    REQUIRE(std::isinf(*v));
    REQUIRE(*v < 0);
}
