//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <algorithm>
#include <string_view>

namespace ParserTools
{
    struct FindSubstring
    {
    public:
        FindSubstring() = default;

        FindSubstring(std::string_view str)
            : m_Substring(str)
        {}

        std::pair<size_t, size_t> operator()(std::string_view str) const
        {
            if (m_Substring.empty())
                return {str.size(), str.size()};
            auto it = std::search(str.begin(), str.end(),
                                  m_Substring.begin(), m_Substring.end());
            auto start = std::distance(str.begin(), it);
            auto end = it != str.end() ? start + m_Substring.size() : start;
            return {start, end};
        }
    private:
        std::string_view m_Substring;
    };

    struct FindNewline
    {
    public:
        std::pair<size_t, size_t> operator()(std::string_view str) const
        {
            auto from = find_if(str.begin(), str.end(),
                                [](char c){return c == '\n' || c == '\r';});
            auto to = from;
            if (to != str.end())
            {
                if (*to++ == '\r' && to != str.end() && *to == '\n')
                    ++to;
            }
            return {std::distance(str.begin(), from),
                    std::distance(str.begin(), to)};
        }
    };

    struct FindSequenceOf
    {
    public:
        FindSequenceOf() = default;

        FindSequenceOf(std::string_view characters)
            : m_Characters(characters)
        {}

        std::pair<size_t, size_t> operator()(std::string_view str) const
        {
            auto match = [this](char c)
                {
                    return std::find(m_Characters.begin(),
                                     m_Characters.end(),
                                     c) != m_Characters.end();
                };
            auto from = std::find_if(str.begin(), str.end(), match);
            auto to = std::find_if_not(from, str.end(), match);
            return {std::distance(str.begin(), from),
                    std::distance(str.begin(), to)};
        }

    private:
        std::string_view m_Characters;
    };
}
