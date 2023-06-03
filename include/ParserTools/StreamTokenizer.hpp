//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cassert>
#include <istream>
#include <string_view>

namespace ParserTools
{
    class StreamTokenizerItem
    {
    public:
        constexpr StreamTokenizerItem() = default;

        constexpr StreamTokenizerItem(std::string_view str,
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

        friend constexpr bool
        operator==(const StreamTokenizerItem& a, const StreamTokenizerItem& b)
        {
            return a.token_start_ == b.token_start_
                   && a.token_end_ == b.token_end_
                   && a.str_.data() == b.str_.data()
                   && a.str_.size() == b.str_.size();
        }

        friend constexpr bool
        operator!=(const StreamTokenizerItem& a, const StreamTokenizerItem& b)
        {
            return !(a == b);
        }

    private:
        std::string_view str_;
        size_t token_start_ = 0;
        size_t token_end_ = 0;
    };

    constexpr size_t DEFAULT_STREAM_BUFFER_CAPACITY = 64 * 1024;

    class StreamBuffer
    {
    public:
        constexpr StreamBuffer() = default;

        explicit constexpr StreamBuffer(std::istream* stream)
            : stream_(stream)
        {}

        StreamBuffer(StreamBuffer&& other) noexcept
            : stream_(other.stream_),
              buffer_(std::move(other.buffer_)),
              offset_(other.offset_),
              size_(other.size_),
              capacity_(other.capacity_)
        {
            other.stream_ = nullptr;
            other.offset_ = other.size_ = other.capacity_ = 0;
        }

        StreamBuffer& operator=(StreamBuffer&& other) noexcept
        {
            stream_ = other.stream_;
            buffer_ = std::move(other.buffer_);
            offset_ = other.offset_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.stream_ = nullptr;
            other.offset_ = other.size_ = other.capacity_ = 0;
            return *this;
        }

        [[nodiscard]]
        constexpr std::string_view string() const
        {
            return {buffer_.get() + offset_, size_ - offset_};
        }

        constexpr void consume(size_t size)
        {
            offset_ += size;
        }

        bool fill()
        {
            if (!stream_ || !*stream_)
                return false;

            if (offset_ != 0)
            {
                std::copy(buffer_.get() + offset_, buffer_.get() + size_,
                          buffer_.get());
                size_ -= offset_;
                offset_ = 0;
            }
            else if (size_ == capacity_)
            {
                capacity_ = std::max(capacity_ * 2, DEFAULT_STREAM_BUFFER_CAPACITY);
                std::unique_ptr<char[]> buffer(new char[capacity_]);
                std::copy(buffer_.get(), buffer_.get() + size_, buffer.get());
                buffer_ = std::move(buffer);
            }

            auto bytes_to_read = std::streamsize(capacity_ - size_);
            stream_->read(buffer_.get() + size_, bytes_to_read);
            auto bytes_read = size_t(stream_->gcount());
            size_ += bytes_read;
            return bytes_read != 0;
        }
    private:
        std::istream* stream_ = nullptr;
        std::unique_ptr<char[]> buffer_;
        size_t offset_ = 0;
        size_t size_ = 0;
        size_t capacity_ = 0;
    };

    template <typename FindDelimiterFunc>
    class StreamTokenizerIterator
    {
    public:
        using difference_type = void;
        using value_type = StreamTokenizerItem;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::input_iterator_tag;

        constexpr StreamTokenizerIterator() = default;

        constexpr StreamTokenizerIterator(std::istream& stream,
                                          FindDelimiterFunc find_func)
            : buffer_(&stream),
              find_delimiter_func_(find_func)
        {
            bool is_first = bool(stream);
            operator++();
            is_first_ = is_first;
        }

        const StreamTokenizerItem& operator*() const
        {
            return item_;
        }

        const StreamTokenizerItem* operator->() const
        {
            return &item_;
        }

        StreamTokenizerIterator& operator++()
        {
            while (true)
            {
                auto[s, e] = find_delimiter_func_(buffer_.string());
                if (e != buffer_.string().size() || !buffer_.fill())
                {
                    item_ = {{buffer_.string()}, s, e};
                    buffer_.consume(e);
                    break;
                }
            }
            is_first_ = false;
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
            return a.is_first_ == b.is_first_ && !a.item_ && !b.item_;
        }

        friend constexpr bool
        operator!=(const StreamTokenizerIterator& a, const StreamTokenizerIterator& b)
        {
            return !(a == b);
        }

    private:
        StreamTokenizerItem item_;
        StreamBuffer buffer_;
        FindDelimiterFunc find_delimiter_func_;
        bool is_first_ = false;
    };

    template <typename FindDelimiterFunc>
    class StreamTokenizer
    {
    public:
        StreamTokenizer(std::istream& stream,
                        FindDelimiterFunc find_delimiter_func)
            : stream_(&stream),
              find_delimiter_func(find_delimiter_func)
        {}

        StreamTokenizerIterator<FindDelimiterFunc> begin()
        {
            if (!stream_)
                throw std::runtime_error("Can not call begin() more than once.");
            auto* stream = stream_;
            stream_ = nullptr;
            return {*stream, find_delimiter_func};
        }

        constexpr StreamTokenizerIterator<FindDelimiterFunc> end() const
        {
            return {};
        }
    private:
        std::istream* stream_;
        FindDelimiterFunc find_delimiter_func;
    };

    template <typename FindDelimiterFunc>
    constexpr StreamTokenizer<FindDelimiterFunc>
    tokenize(std::istream& stream, FindDelimiterFunc find_delimiter_func)
    {
        using std::move;
        return {stream, move(find_delimiter_func)};
    }
}
