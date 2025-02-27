//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>

namespace ParserTools
{
    namespace Details
    {
        int get_digit(char c)
        {
            return int(uint8_t(c) ^ 0x30u);
        }
    }

    template <typename T>
    std::optional<T> parse_floating_point(std::string_view str)
    {
        if (str.empty())
            return {};

        size_t i = 0;
        // Get the sign of the number
        bool negative = false;
        if (str[0] == '-')
        {
            negative = true;
            if (++i == str.size())
                return {};
        }
        else if (str[0] == '+')
        {
            if (++i == str.size())
                return {};
        }

        // Get the integer value
        auto value = T(Details::get_digit(str[i]));
        if (value > 9)
        {
            if (str == "Infinity" || str == "null" || str == "+Infinity")
                return std::numeric_limits<T>::infinity();
            if (str == "-Infinity")
                return -std::numeric_limits<T>::infinity();
            if (str == "NaN")
                return std::numeric_limits<T>::quiet_NaN();
            return {};
        }

        bool underscore = false;
        for (++i; i < str.size(); ++i)
        {
            auto digit = Details::get_digit(str[i]);
            if (digit <= 9)
            {
                value *= 10;
                value += digit;
                underscore = false;
            }
            else if (str[i] != '_' || underscore)
            {
                break;
            }
            else
            {
                underscore = true;
            }
        }

        if (underscore)
            return {};

        if (i == str.size())
            return !negative ? value : -value;

        // Get the fraction
        underscore = true; // Makes underscore after point illegal.
        int decimals = 0;
        T fraction = {};
        if (str[i] == '.')
        {
            for (++i; i < str.size(); ++i)
            {
                auto digit = Details::get_digit(str[i]);
                if (digit <= 9)
                {
                    fraction *= 10;
                    fraction += digit;
                    underscore = false;
                    ++decimals;
                }
                else if (str[i] != '_' || underscore)
                {
                    break;
                }
                else
                {
                    underscore = true;
                }
            }
        }

        // Get the exponent
        int exponent = 0;
        if (i != str.size())
        {
            if ((uint8_t(str[i]) & 0xDFu) != 'E')
                return {};

            if (++i == str.size())
                return {};

            bool negative_exponent = false;
            if (str[i] == '-')
            {
                negative_exponent = true;
                if (++i == str.size())
                    return {};
            }
            else if (str[i] == '+')
            {
                if (++i == str.size())
                    return {};
            }

            exponent += Details::get_digit(str[i]);
            if (exponent > 9)
                return {};

            for (++i; i != str.size(); ++i)
            {
                auto digit = Details::get_digit(str[i]);
                if (digit <= 9)
                {
                    exponent *= 10;
                    exponent += digit;
                    underscore = false;
                }
                else if (str[i] != '_' || underscore)
                {
                    return {};
                }
                else
                {
                    underscore = true;
                }
                if (exponent > std::numeric_limits<T>::max_exponent10)
                    return {};
            }
            if (negative_exponent)
                exponent = -exponent;
        }

        if (fraction)
            value += fraction * std::pow(T(10), T(-decimals));
        if (exponent)
            value *= std::pow(T(10), T(exponent));

        // Add the sign
        if (negative)
            value = -value;

        return value;
    }
}
