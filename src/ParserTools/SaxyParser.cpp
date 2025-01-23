//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-12-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserTools/SaxyParser.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <expat.h>

namespace ParserTools
{
    namespace Details
    {
        struct ParserContext
        {
            ~ParserContext()
            {
                if (parser)
                    XML_ParserFree(parser);
            }

            XML_Parser parser = nullptr;
            ElementHandler* handler = nullptr;
            std::string buffer;
            bool ignore_whitespace = true;
        };
    }

    namespace
    {
        bool is_whitespace(std::string_view s)
        {
            return std::all_of(s.begin(), s.end(), [](char c)
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            });
        }

        void handle_character_data_buffer(Details::ParserContext& context)
        {
            if (context.buffer.empty())
                return;

            if (!context.ignore_whitespace || !is_whitespace(context.buffer))
                context.handler->character_data(context.buffer);
            context.buffer.clear();
        }

        size_t count_attributes(const XML_Char** attributes)
        {
            size_t count = 0;
            while (*attributes)
            {
                ++count;
                attributes += 2;
            }
            return count;
        }

        void XMLCALL start_element_handler(void* user_data,
                                           const XML_Char* name,
                                           const XML_Char** attributes)
        {
            auto& context = *static_cast<Details::ParserContext*>(user_data);
            handle_character_data_buffer(context);

            std::vector<ElementHandler::Attribute> attrs;
            attrs.reserve(count_attributes(attributes));
            while (*attributes)
            {
                attrs.emplace_back(attributes[0], attributes[1]);
                attributes += 2;
            }

            context.handler->start_element(name, attrs);
        }

        void XMLCALL end_element_handler(void* user_data,
                                         const XML_Char* name)
        {
            auto& context = *static_cast<Details::ParserContext*>(user_data);
            handle_character_data_buffer(context);
            context.handler->end_element(name);
        }

        void XMLCALL character_data_handler(void* user_data,
                                            const XML_Char* s,
                                            int len)
        {
            auto& context = *static_cast<Details::ParserContext*>(user_data);
            context.buffer.append(s, size_t(len));
        }

        XML_Parser create_parser(Details::ParserContext& context)
        {
            auto parser = XML_ParserCreate("UTF-8");
            if (!parser)
                EXPAT_THROW("Failed to create XML parser_.");
            XML_SetUserData(parser, &context);
            XML_SetElementHandler(parser,
                                  start_element_handler,
                                  end_element_handler);
            XML_SetCharacterDataHandler(parser,
                                        character_data_handler);
            return parser;
        }
    }

    SaxyParser::SaxyParser() = default;

    SaxyParser::SaxyParser(ElementHandler& handler)
        : context_(std::make_unique<Details::ParserContext>())
    {
        context_->parser = create_parser(*context_);
        context_->handler = &handler;
    }

    SaxyParser::SaxyParser(SaxyParser&&) noexcept = default;

    SaxyParser& SaxyParser::operator=(SaxyParser&&) noexcept = default;

    SaxyParser::~SaxyParser() = default;

    void SaxyParser::parse(const std::string_view& xml, bool is_final)
    {
        if (xml.empty() && is_final)
        {
            parse(xml.data(), 0, true);
            return;
        }

        constexpr size_t CHUNK_SIZE = 16 * 1024 * 1024;
        for (size_t i = 0; i < xml.size(); i += CHUNK_SIZE)
        {
            parse(xml.data() + i, std::min(xml.size() - i, CHUNK_SIZE),
                  is_final && xml.size() <= i + CHUNK_SIZE);
        }
    }

    void SaxyParser::parse(const void* data, size_t size, bool is_final)
    {
        if (!context_ || !context_->handler)
            EXPAT_THROW("Parser has no handler.");

        if (!context_->parser)
            context_->parser = create_parser(*context_);

        if (XML_Parse(context_->parser, static_cast<const char*>(data),
                      int(size), is_final) == XML_STATUS_ERROR)
        {
            auto error = XML_ErrorString(XML_GetErrorCode(context_->parser));
            auto error_line = XML_GetCurrentLineNumber(context_->parser);
            EXPAT_THROW(+ std::string(error) + " [line "
                        + std::to_string(error_line) + "]");
        }
    }

    void SaxyParser::parse(std::istream& stream)
    {
        constexpr size_t CHUNK_SIZE = 16 * 1024;
        std::vector<char> buffer(CHUNK_SIZE);

        bool is_final = false;
        while (!is_final)
        {
            stream.read(buffer.data(), std::streamsize(buffer.size()));
            auto count = stream.gcount();
            is_final = count < std::streamsize(buffer.size());
            parse(buffer.data(), size_t(count), false);
        }
    }

    ElementHandler* SaxyParser::handler() const
    {
        return context_ ? context_->handler : nullptr;
    }

    ElementHandler* SaxyParser::set_handler(ElementHandler* handler)
    {
        if (!context_ && !handler)
            return nullptr;

        if (!context_)
            context_ = std::make_unique<Details::ParserContext>();

        auto old_handler = context_->handler;
        context_->handler = handler;
        return old_handler;
    }

    bool SaxyParser::ignore_whitespace() const
    {
        return !context_ || context_->ignore_whitespace;
    }

    void SaxyParser::set_ignore_whitespace(bool value)
    {
        if (!context_)
            context_ = std::make_unique<Details::ParserContext>();
        context_->ignore_whitespace = value;
    }

    XML_ParserStruct* SaxyParser::expat_parser() const
    {
        return context_ ? context_->parser : nullptr;
    }

    std::string_view get_attribute(const ElementHandler::Attributes& attrs,
                                   std::string_view name)
    {
        auto it = std::find_if(attrs.begin(), attrs.end(),
                               [&](const auto& attr)
                               {
                                   return attr.first == name;
                               });
        return it == attrs.end() ? std::string_view() : it->second;
    }
}
