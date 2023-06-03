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
                            size_t buffer_size = 8192)
        : find_delimiter_func_(std::move(func)),
          buffer_(buffer_size),
          stream_(&stream)
    {}

    bool next()
    {
        str_ = str_.substr(delimiter_end);
        if (str_.empty() && !fill_buffer())
        {
            delimiter_start_ = delimiter_end = 0;
            return false;
        }

        while (true)
        {
            auto[s, e] = find_delimiter_func_(str_);
            if ((s != e && e != str_.size()) || !fill_buffer())
            {
                assert(e <= str_.size());
                assert(s <= e);
                delimiter_start_ = s;
                delimiter_end = e;
                return true;
            }
        }
    }

    [[nodiscard]]
    std::string_view preceding_substring() const
    {
        return str_.substr(0, delimiter_start_);
    }

    [[nodiscard]]
    std::string_view delimiter() const
    {
        return str_.substr(delimiter_start_,
                           delimiter_end - delimiter_start_);
    }

    [[nodiscard]]
    std::string_view remaining_buffer() const
    {
        return str_.substr(delimiter_end);
    }
private:
    bool fill_buffer()
    {
        if (!stream_ || !*stream_)
            return false;

        if (delimiter_end != 0)
        {
            std::copy(buffer_.begin() + delimiter_end, buffer_.end(),
                      buffer_.begin());
            str_ = {buffer_.data(), buffer_.size() - delimiter_end};
            delimiter_start_ = delimiter_end = 0;
        }
        else if (str_.size() == buffer_.size())
        {
            auto prev_size = buffer_.size();
            buffer_.resize(buffer_.size() * 2);
            str_ = {buffer_.data(), prev_size};
        }

        auto bytes_to_read = std::streamsize(buffer_.size() - str_.size());
        stream_->read(buffer_.data() + str_.size(), bytes_to_read);
        auto bytes_read = size_t(stream_->gcount());
        buffer_.resize(str_.size() + bytes_read);
        str_ = {buffer_.data(), buffer_.size()};
        return bytes_read != 0;
    }

    std::istream* stream_ = nullptr;
    std::string_view str_;
    size_t delimiter_start_ = 0;
    size_t delimiter_end = 0;
    std::vector<char> buffer_;
    FindDelimiterFunc find_delimiter_func_;
};
