//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-08-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cassert>
#include <istream>
#include <string_view>
#include <vector>

template <typename FindDelimiterFunc>
class StreamDelimiterIterator
{
public:
    StreamDelimiterIterator() = default;

    StreamDelimiterIterator(std::istream& stream, FindDelimiterFunc func,
                            size_t bufferSize = 8192)
        : m_FindDelimiterFunc(func),
          m_Buffer(bufferSize),
          m_Stream(&stream)
    {}

    bool next()
    {
        m_Str = m_Str.substr(m_DelimiterEnd);
        if (m_Str.empty() && !fill_buffer())
        {
            m_DelimiterStart = m_DelimiterEnd = 0;
            return false;
        }
        while (true)
        {
            auto[s, e] = m_FindDelimiterFunc(m_Str);
            if ((s != e && e != m_Str.size()) || !fill_buffer())
            {
                assert(e <= m_Str.size());
                assert(s <= e);
                m_DelimiterStart = s;
                m_DelimiterEnd = e;
                return true;
            }
        }
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
    std::string_view remaining_buffer() const
    {
        return m_Str.substr(m_DelimiterEnd);
    }
private:
    bool fill_buffer()
    {
        if (!m_Stream || !*m_Stream)
            return false;
        if (m_DelimiterEnd != 0)
        {
            std::copy(m_Buffer.begin() + m_DelimiterEnd, m_Buffer.end(),
                      m_Buffer.begin());
            m_Str = {m_Buffer.data(), m_Buffer.size() - m_DelimiterEnd};
            m_DelimiterStart = m_DelimiterEnd = 0;
        }
        else if (m_Str.size() == m_Buffer.size())
        {
            auto prevSize = m_Buffer.size();
            m_Buffer.resize(m_Buffer.size() * 2);
            m_Str = {m_Buffer.data(), prevSize};
        }
        auto bytesToRead = m_Buffer.size() - m_Str.size();
        m_Stream->read(m_Buffer.data() + m_Str.size(), bytesToRead);
        auto bytesRead = size_t(m_Stream->gcount());
        m_Buffer.resize(m_Str.size() + bytesRead);
        m_Str = {m_Buffer.data(), m_Buffer.size()};
        return bytesRead != 0;
    }

    std::istream* m_Stream = nullptr;
    std::string_view m_Str;
    size_t m_DelimiterStart = 0;
    size_t m_DelimiterEnd = 0;
    std::vector<char> m_Buffer;
    FindDelimiterFunc m_FindDelimiterFunc;
};
