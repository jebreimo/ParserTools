//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 23.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>

namespace ParserTools
{
    namespace Details
    {
        template <typename IntT>
        IntT from_digit(char c)
        {
            if ('0' <= c && c <= '9')
                return IntT(c - '0');
            auto u = uint8_t(c);
            u &= 0xDFu;
            if ('A' <= u && u <= 'Z')
                return IntT(10 + u - 'A');
            return std::numeric_limits<IntT>::max();
        }

        template <typename T, T Base>
        constexpr T max_preceding_value_negative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) / U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T max_final_digit_negative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) % U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T max_preceding_value_positive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) / U(Base));
            }
            else
            {
                return T(~T(0)) / Base;
            }
        }

        template <typename T, T Base>
        constexpr T max_final_digit_positive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) % U(Base));
            }
            else
            {
                return T(~T(0)) % Base;
            }
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parse_positive_integer_impl(std::string_view str)
        {
            if (str.empty())
                return {};
            IntT value = from_digit<IntT>(str[0]);
            if (value >= Base)
                return {};
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = from_digit<IntT>(str[i]);
                if (digit < Base
                    && (value < max_preceding_value_positive<IntT, Base>()
                        || (value == max_preceding_value_positive<IntT, Base>()
                            && digit <= max_final_digit_positive<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return {};
                }
            }
            return value;
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parse_negative_integer_impl(std::string_view str)
        {
            if constexpr (std::is_signed<IntT>::value)
            {
                if (str.empty())
                    return {};
                IntT value = from_digit<IntT>(str[0]);
                if (value >= Base)
                    return {};
                for (size_t i = 1; i < str.size(); ++i)
                {
                    auto digit = from_digit<IntT>(str[i]);
                    if (digit < Base
                        && (value < max_preceding_value_negative<IntT, Base>()
                            || (value == max_preceding_value_negative<IntT, Base>()
                                && digit <= max_final_digit_negative<IntT, Base>())))
                    {
                        value *= Base;
                        value += digit;
                    }
                    else if (str[i] != '_' || i == str.size() - 1
                             || str[i - 1] == '_')
                    {
                        return {};
                    }
                }
                return IntT(-value);
            }
            else
            {
                if (str.empty())
                    return {};
                for (char c : str)
                {
                    auto digit = from_digit<IntT>(c);
                    if (digit > 0)
                        return {};
                }
                return IntT(0);
            }
        }
    }

    template <typename IntT>
    std::optional<IntT> parse_integer(std::string_view str, bool detect_base)
    {
        static_assert(std::is_integral<IntT>());

        if (str.empty())
            return {};

        bool positive = true;
        if (str[0] == '-')
        {
            positive = false;
            str = str.substr(1);
        }
        else if (str[0] == '+')
        {
            str = str.substr(1);
        }

        if (str.empty())
            return {};

        if (str[0] == '0' && str.size() >= 3 && detect_base)
        {
            auto str2 = str.substr(2);
            switch (uint8_t(str[1]) | 0x20u)
            {
            case 'b':
                return positive ? Details::parse_positive_integer_impl<IntT, 2>(str2)
                                : Details::parse_negative_integer_impl<IntT, 2>(str2);
            case 'o':
                return positive ? Details::parse_positive_integer_impl<IntT, 8>(str2)
                                : Details::parse_negative_integer_impl<IntT, 8>(str2);
            case 'x':
                return positive ? Details::parse_positive_integer_impl<IntT, 16>(str2)
                                : Details::parse_negative_integer_impl<IntT, 16>(str2);
            default:
                break;
            }
        }
        if ('0' <= str[0] && str[0] <= '9')
        {
            return positive ? Details::parse_positive_integer_impl<IntT, 10>(str)
                            : Details::parse_negative_integer_impl<IntT, 10>(str);
        }
        if (str == "false" || str == "null")
            return IntT(0);
        if (str == "true")
            return IntT(1);
        return {};
    }
}
