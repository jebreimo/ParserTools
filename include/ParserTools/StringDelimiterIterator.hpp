//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cassert>
#include <string_view>

namespace ParserTools
{
    template <typename FindDelimiterFunc>
    class StringDelimiterIterator
    {
    public:
        StringDelimiterIterator() = default;

        StringDelimiterIterator(std::string_view str, FindDelimiterFunc func)
            : m_Str(str),
              m_FindDelimiterFunc(func)
        {}

        bool next()
        {
            m_Str = m_Str.substr(m_DelimiterEnd);
            if (m_Str.empty())
            {
                m_DelimiterStart = m_DelimiterEnd = 0;
                return false;
            }
            auto[s, e] = m_FindDelimiterFunc(m_Str);
            assert(s <= e);
            assert(e <= m_Str.size());
            m_DelimiterStart = s;
            m_DelimiterEnd = e;
            return true;
        }

        [[nodiscard]]
        std::string_view preceding_substring() const
        {
            return m_Str.substr(0, m_DelimiterStart);
        }

        [[nodiscard]]
        std::string_view delimiter() const
        {
            return m_Str.substr(m_DelimiterStart,
                                m_DelimiterEnd - m_DelimiterStart);
        }

        [[nodiscard]]
        std::string_view remaining_substring() const
        {
            return m_Str.substr(m_DelimiterEnd);
        }

    private:
        std::string_view m_Str;
        size_t m_DelimiterStart = 0;
        size_t m_DelimiterEnd = 0;
        FindDelimiterFunc m_FindDelimiterFunc;
    };
}
