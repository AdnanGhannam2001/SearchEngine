#include "../includes/xml-parser.hpp"

const std::unordered_set<std::string> SearchEngine::XmlParser::s_ignore_tags = { "head", "style", "script", "footer" };

SearchEngine::XmlParser::XmlParser(const std::string &filename)
    : m_reader(xmlReaderForFile(filename.c_str(), NULL, 0))
{
}

std::string
SearchEngine::XmlParser::parse()
{
    int ret;
    std::string content;

    while ((ret = xmlTextReaderRead(m_reader)) == 1)
    {
        const xmlChar *name = xmlTextReaderConstName(m_reader);
        const int type = xmlTextReaderNodeType(m_reader);

        if (std::find(s_ignore_tags.begin(), s_ignore_tags.end(), (char*) name) != s_ignore_tags.end())
        {
            ret = xmlTextReaderNext(m_reader);
            continue;
        }

        if (type == XML_READER_TYPE_TEXT)
        {
            content += (char*) xmlTextReaderConstValue(m_reader);
            content += " ";
        }
    }

    return content;
}

SearchEngine::XmlParser::~XmlParser()
{
    xmlFreeTextReader(m_reader);
}