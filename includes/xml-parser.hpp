#ifndef SEARCH_ENGINE_XML_PARSER_HPP
#define SEARCH_ENGINE_XML_PARSER_HPP

#include "common.hpp"
#include <libxml/xmlreader.h>

namespace SearchEngine
{
    class XmlParser
    {
    private:
        static const std::unordered_set<std::string> s_ignore_tags;

        xmlTextReaderPtr m_reader;

    public:
        XmlParser(const std::string &filename);

        std::string parse();

        ~XmlParser();
    };
}

#endif // SEARCH_ENGINE_XML_PARSER_HPP