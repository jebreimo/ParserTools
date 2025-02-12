//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-12-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

/**
 * @file
 * @brief Defines the SaxPatParser class.
 *
 * SaxPatParser is a thin SAX-like wrapper around the Expat XML parser.
 */

// Forward declaration of Expat's parser struct.
struct XML_ParserStruct;

namespace ParserTools
{
    /**
     * @brief An interface for handling XML elements.
     *
     * The handler is called when the parser_ encounters an XML element.
     * The handler is also called when the parser_ encounters character data
     * between two elements.
     */
    class ElementHandler
    {
    public:
        using Attribute = std::pair<std::string_view, std::string_view>;
        using Attributes = std::vector<Attribute>;

        virtual ~ElementHandler() = default;

        virtual void start_element(std::string_view name,
                                   const Attributes& attributes)
        {}

        virtual void end_element(std::string_view name)
        {}

        virtual void character_data(std::string_view text)
        {}
    };

    namespace Details
    {
        struct ParserContext;
    }

    /**
     * @brief A SAX-like XML parser.
     *
     * The parser is a fairly thin wrapper around Expat.
     */
    class SaxPatParser
    {
    public:
        SaxPatParser();

        explicit SaxPatParser(ElementHandler& handler);

        SaxPatParser(const SaxPatParser&) = delete;

        SaxPatParser(SaxPatParser&&) noexcept;

        SaxPatParser& operator=(const SaxPatParser&) = delete;

        SaxPatParser& operator=(SaxPatParser&&) noexcept;

        ~SaxPatParser();

        void parse(const std::string_view& xml, bool is_final = true);

        void parse(const void* data, size_t size, bool is_final = true);

        void parse(std::istream& stream);

        void stop(bool resumable = false);

        void reset();

        [[nodiscard]] ElementHandler* handler() const;

        ElementHandler* set_handler(ElementHandler* handler);

        [[nodiscard]] bool ignore_whitespace() const;

        void set_ignore_whitespace(bool value);

        [[nodiscard]] XML_ParserStruct* expat_parser() const;
    private:
        std::unique_ptr<Details::ParserContext> context_;
    };

    class SaxPatException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    std::string_view get_attribute(const ElementHandler::Attributes& attrs,
                                   std::string_view name);

    #ifdef _MSC_VER
        #define SAXPAT_THROW_2_(file, line, msg) \
            throw ::ParserTools::SaxPatException(file "(" #line "): " msg)
    #else
        #define SAXPAT_THROW_2_(file, line, msg) \
            throw ::ParserTools::SaxPatException(file ":" #line ": " msg)
    #endif

    #define SAXPAT_THROW_1_(file, line, msg) \
        SAXPAT_THROW_2_(file, line, msg)

    #define SAXPAT_THROW(msg) \
        SAXPAT_THROW_1_(__FILE__, __LINE__, msg)
}
