//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string_view>

namespace ParserTools
{
    class StringTokenizerItem
    {
    public:
        constexpr StringTokenizerItem() = default;

        constexpr StringTokenizerItem(std::string_view str,
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

        [[nodiscard]]
        constexpr std::string_view remainder() const
        {
            return m_Str.substr(m_TokenEnd);
        }

        friend constexpr bool
        operator==(const StringTokenizerItem& a, const StringTokenizerItem& b)
        {
            return a.m_TokenStart == b.m_TokenStart
                   && a.m_TokenEnd == b.m_TokenEnd
                   && a.m_Str.data() == b.m_Str.data()
                   && a.m_Str.size() == b.m_Str.size();
        }

        friend constexpr bool
        operator!=(const StringTokenizerItem& a, const StringTokenizerItem& b)
        {
            return !(a == b);
        }
    private:
        std::string_view m_Str;
        size_t m_TokenStart = 0;
        size_t m_TokenEnd = 0;
    };

    template <typename FindDelimiterFunc>
    class StringTokenizerIterator
    {
    public:
        using difference_type = void;
        using value = StringTokenizerItem;
        using reference = const value&;
        using pointer = const value*;
        using iterator_category = std::forward_iterator_tag;

        constexpr StringTokenizerIterator() = default;

        constexpr StringTokenizerIterator(std::string_view str,
                                          FindDelimiterFunc findFunc)
            : m_FindDelimiterFunc(move(findFunc))
        {
            auto[s, e] = m_FindDelimiterFunc(str);
            m_Item = {str, s, e};
        }

        const StringTokenizerItem& operator*() const
        {
            return m_Item;
        }

        const StringTokenizerItem* operator->() const
        {
            return &m_Item;
        }

        StringTokenizerIterator& operator++()
        {
            auto[s, e] = m_FindDelimiterFunc(m_Item.remainder());
            m_Item = {m_Item.remainder(), s, e};
            return *this;
        }

        StringTokenizerIterator operator++(int)
        {
            auto prev = *this;
            auto[s, e] = m_FindDelimiterFunc(m_Item.remainder());
            m_Item = {m_Item.remainder(), s, e};
            return prev;
        }

        friend constexpr bool operator==(const StringTokenizerIterator& a, const StringTokenizerIterator& b)
        {
            return (!a.m_Item && !b.m_Item) || (a.m_Item == b.m_Item);
        }

        friend constexpr bool operator!=(const StringTokenizerIterator& a, const StringTokenizerIterator& b)
        {
            return !(a == b);
        }
    private:
        StringTokenizerItem m_Item;
        FindDelimiterFunc m_FindDelimiterFunc;
    };

    template <typename FindDelimiterFunc>
    class StringTokenizer
    {
    public:
        StringTokenizer(std::string_view str,
                        FindDelimiterFunc findDelimiterFunc)
            : m_Str(str),
              m_FindDelimiterFunc(findDelimiterFunc)
        {
            using std::move;
        }

        constexpr StringTokenizerIterator<FindDelimiterFunc> begin() const
        {
            return {m_Str, m_FindDelimiterFunc};
        }

        constexpr StringTokenizerIterator<FindDelimiterFunc> end() const
        {
            return {};
        }
    private:
        std::string_view m_Str;
        FindDelimiterFunc m_FindDelimiterFunc;
    };

    template <typename FindDelimiterFunc>
    constexpr StringTokenizer<FindDelimiterFunc>
    tokenize(std::string_view str, FindDelimiterFunc findDelimiterFunc)
    {
        using std::move;
        return {str, move(findDelimiterFunc)};
    }
}