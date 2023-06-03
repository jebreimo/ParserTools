//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cassert>
#include <string_view>
#include <vector>

namespace ParserTools
{
    class StringTokenizerItem
    {
    public:
        constexpr StringTokenizerItem() = default;

        constexpr StringTokenizerItem(std::string_view str,
                                      size_t token_start, size_t token_end)
            : str_(str),
              token_start_(token_start),
              token_end_(token_end)
        {
            assert(token_start <= token_end);
            assert(token_end <= str.size());
        }

        explicit constexpr operator bool() const
        {
            return !str_.empty();
        }

        [[nodiscard]]
        constexpr std::string_view string() const
        {
            return str_.substr(0, token_start_);
        }

        [[nodiscard]]
        constexpr std::string_view token() const
        {
            return str_.substr(token_start_, token_end_ - token_start_);
        }

        [[nodiscard]]
        constexpr std::string_view remainder() const
        {
            return str_.substr(token_end_);
        }

        friend constexpr bool
        operator==(const StringTokenizerItem& a, const StringTokenizerItem& b)
        {
            return a.token_start_ == b.token_start_
                   && a.token_end_ == b.token_end_
                   && a.str_.data() == b.str_.data()
                   && a.str_.size() == b.str_.size();
        }

        friend constexpr bool
        operator!=(const StringTokenizerItem& a, const StringTokenizerItem& b)
        {
            return !(a == b);
        }
    private:
        std::string_view str_;
        size_t token_start_ = 0;
        size_t token_end_ = 0;
    };

    template <typename FindDelimiterFunc>
    class StringTokenizerIterator
    {
    public:
        using difference_type = ptrdiff_t;
        using value_type = StringTokenizerItem;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::forward_iterator_tag;

        constexpr StringTokenizerIterator() = default;

        constexpr StringTokenizerIterator(std::string_view str,
                                          FindDelimiterFunc find_func)
            : find_delimiter_func_(find_func),
              is_first_(true)
        {
            auto [s, e] = find_delimiter_func_(str);
            item_ = {str, s, e};
        }

        const StringTokenizerItem& operator*() const
        {
            return item_;
        }

        const StringTokenizerItem* operator->() const
        {
            return &item_;
        }

        StringTokenizerIterator& operator++()
        {
            auto[s, e] = find_delimiter_func_(item_.remainder());
            item_ = {item_.remainder(), s, e};
            is_first_ = false;
            return *this;
        }

        StringTokenizerIterator operator++(int)
        {
            auto prev = *this;
            auto [s, e] = find_delimiter_func_(item_.remainder());
            item_ = {item_.remainder(), s, e};
            is_first_ = false;
            return prev;
        }

        friend constexpr bool operator==(const StringTokenizerIterator& a, const StringTokenizerIterator& b)
        {
            return a.is_first_ == b.is_first_ && ((!a.item_ && !b.item_) || (a.item_ == b.item_));
        }

        friend constexpr bool operator!=(const StringTokenizerIterator& a, const StringTokenizerIterator& b)
        {
            return !(a == b);
        }
    private:
        StringTokenizerItem item_;
        FindDelimiterFunc find_delimiter_func_;
        bool is_first_ = false;
    };

    template <typename FindDelimiterFunc>
    class StringTokenizer
    {
    public:
        StringTokenizer(std::string_view str,
                        FindDelimiterFunc find_delimiter_func)
            : str_(str),
              find_delimiter_func_(find_delimiter_func)
        {}

        constexpr StringTokenizerIterator<FindDelimiterFunc> begin() const
        {
            return {str_, find_delimiter_func_};
        }

        constexpr StringTokenizerIterator<FindDelimiterFunc> end() const
        {
            return {};
        }
    private:
        std::string_view str_;
        FindDelimiterFunc find_delimiter_func_;
    };

    template <typename FindDelimiterFunc>
    constexpr StringTokenizer<FindDelimiterFunc>
    tokenize(std::string_view str, FindDelimiterFunc find_delimiter_func)
    {
        return {str, std::move(find_delimiter_func)};
    }

    template <typename FindDelimiterFunc>
    std::vector<std::string_view>
    split(std::string_view str, FindDelimiterFunc find_delimiter_func, size_t maxSplits = SIZE_MAX)
    {
        if (maxSplits == 0)
          return {str};

        std::vector<std::string_view> result;
        auto tokenizer = tokenize(str, std::move(find_delimiter_func));
        for (auto item : tokenizer)
        {
            result.push_back(item.string());
            if (--maxSplits == 0 || (item.remainder().empty() && !item.token().empty()))
            {
              result.push_back(item.remainder());
              break;
            }
        }
        return result;
    }
}
