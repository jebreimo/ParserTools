//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <istream>
#include <string_view>

namespace ParserTools
{
    class StreamTokenizerItem
    {
    public:
        constexpr StreamTokenizerItem() = default;

        constexpr StreamTokenizerItem(std::string_view str,
                                      size_t tokenStart, size_t tokenEnd)
            : m_Str(str),
              m_TokenStart(tokenStart),
              m_TokenEnd(tokenEnd)
        {
            assert(tokenStart <= tokenEnd);
            assert(tokenEnd <= str.size());
        }

        explicit constexpr operator bool() const
        {
            return !m_Str.empty();
        }

        constexpr std::string_view operator*() const
        {
            return m_Str.substr(0, m_TokenStart);
        }

        [[nodiscard]]
        constexpr std::string_view string() const
        {
            return m_Str.substr(0, m_TokenStart);
        }

        [[nodiscard]]
        constexpr std::string_view token() const
        {
            return m_Str.substr(m_TokenStart, m_TokenEnd - m_TokenStart);
        }

        friend constexpr bool
        operator==(const StreamTokenizerItem& a, const StreamTokenizerItem& b)
        {
            return a.m_TokenStart == b.m_TokenStart
                   && a.m_TokenEnd == b.m_TokenEnd
                   && a.m_Str.data() == b.m_Str.data()
                   && a.m_Str.size() == b.m_Str.size();
        }

        friend constexpr bool
        operator!=(const StreamTokenizerItem& a, const StreamTokenizerItem& b)
        {
            return !(a == b);
        }

    private:
        std::string_view m_Str;
        size_t m_TokenStart = 0;
        size_t m_TokenEnd = 0;
    };

    constexpr size_t DEFAULT_STREAM_BUFFER_CAPACITY = 64 * 1024;

    class StreamBuffer
    {
    public:
        StreamBuffer() = default;

        explicit StreamBuffer(std::istream* stream)
            : m_Stream(stream)
        {}

        StreamBuffer(StreamBuffer&& other) noexcept
            : m_Stream(other.m_Stream),
              m_Buffer(move(other.m_Buffer)),
              m_Offset(other.m_Offset),
              m_Size(other.m_Size),
              m_Capacity(other.m_Capacity)
        {
            other.m_Stream = nullptr;
            other.m_Offset = other.m_Size = other.m_Capacity = 0;
        }

        StreamBuffer& operator=(StreamBuffer&& other) noexcept
        {
            m_Stream = other.m_Stream;
            m_Buffer = move(other.m_Buffer);
            m_Offset = other.m_Offset;
            m_Size = other.m_Size;
            m_Capacity = other.m_Capacity;
            other.m_Stream = nullptr;
            other.m_Offset = other.m_Size = other.m_Capacity = 0;
            return *this;
        }

        [[nodiscard]]
        std::string_view string() const
        {
            return {m_Buffer.get() + m_Offset, m_Size - m_Offset};
        }

        void consume(size_t size)
        {
            m_Offset += size;
        }

        bool fill()
        {
            if (!m_Stream || !*m_Stream)
                return false;
            if (m_Offset != 0)
            {
                std::copy(m_Buffer.get() + m_Offset, m_Buffer.get() + m_Size,
                          m_Buffer.get());
                m_Size -= m_Offset;
                m_Offset = 0;
            }
            else if (m_Size == m_Capacity)
            {
                m_Capacity = std::max(m_Capacity * 2, DEFAULT_STREAM_BUFFER_CAPACITY);
                std::unique_ptr<char[]> buffer(new char[m_Capacity]);
                std::copy(m_Buffer.get(), m_Buffer.get() + m_Size, buffer.get());
                m_Buffer = move(buffer);
            }
            auto bytesToRead = m_Capacity - m_Size;
            m_Stream->read(m_Buffer.get() + m_Size, bytesToRead);
            auto bytesRead = size_t(m_Stream->gcount());
            m_Size += bytesRead;
            return bytesRead != 0;
        }
    private:
        std::istream* m_Stream = nullptr;
        std::unique_ptr<char[]> m_Buffer;
        size_t m_Offset = 0;
        size_t m_Size = 0;
        size_t m_Capacity = 0;
    };

    template <typename FindDelimiterFunc>
    class StreamTokenizerIterator
    {
    public:
        using difference_type = void;
        using value = StreamTokenizerItem;
        using reference = const value&;
        using pointer = const value*;
        using iterator_category = std::input_iterator_tag;

        constexpr StreamTokenizerIterator() = default;

        constexpr StreamTokenizerIterator(std::istream& stream,
                                          FindDelimiterFunc findFunc)
            : m_Buffer(&stream),
              m_FindDelimiterFunc(findFunc)
        {
            operator++();
        }

        const StreamTokenizerItem& operator*() const
        {
            return m_Item;
        }

        const StreamTokenizerItem* operator->() const
        {
            return &m_Item;
        }

        StreamTokenizerIterator& operator++()
        {
            while (true)
            {
                auto[s, e] = m_FindDelimiterFunc(m_Buffer.string());
                if (e != m_Buffer.string().size() || !m_Buffer.fill())
                {
                    m_Item = {{m_Buffer.string()}, s, e};
                    m_Buffer.consume(e);
                    break;
                }
            }
            return *this;
        }

        StreamTokenizerIterator operator++(int)
        {
            operator++();
            return {};
        }

        friend constexpr bool
        operator==(const StreamTokenizerIterator& a, const StreamTokenizerIterator& b)
        {
            return !a.m_Item && !b.m_Item;
        }

        friend constexpr bool
        operator!=(const StreamTokenizerIterator& a, const StreamTokenizerIterator& b)
        {
            return !(a == b);
        }

    private:
        StreamTokenizerItem m_Item;
        StreamBuffer m_Buffer;
        FindDelimiterFunc m_FindDelimiterFunc;
    };

    template <typename FindDelimiterFunc>
    class StreamTokenizer
    {
    public:
        StreamTokenizer(std::istream& stream,
                        FindDelimiterFunc findDelimiterFunc)
            : m_Stream(&stream),
              m_FindDelimiterFunc(findDelimiterFunc)
        {
            using std::move;
        }

        StreamTokenizerIterator<FindDelimiterFunc> begin()
        {
            if (!m_Stream)
                throw std::runtime_error("Can not call begin() more than once.");
            auto* stream = m_Stream;
            m_Stream = nullptr;
            return {*stream, m_FindDelimiterFunc};
        }

        constexpr StreamTokenizerIterator<FindDelimiterFunc> end() const
        {
            return {};
        }
    private:
        std::istream* m_Stream;
        FindDelimiterFunc m_FindDelimiterFunc;
    };

    template <typename FindDelimiterFunc>
    constexpr StreamTokenizer<FindDelimiterFunc>
    tokenize(std::istream& stream, FindDelimiterFunc findDelimiterFunc)
    {
        using std::move;
        return {stream, move(findDelimiterFunc)};
    }
}
