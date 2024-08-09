#include "../includes/dictionary.hpp"

SearchEngine::Dictionary::Dictionary()
    : m_file_map_ptr(new FileMap()),
      m_term_occurrence_map_ptr(new TermOccurrenceMap())
{
}

void
SearchEngine::Dictionary::write_to_xml(const std::string& output_filename)
const
{
    xmlTextWriterPtr writer = xmlNewTextWriterFilename(output_filename.c_str(), 0);

    xmlTextWriterStartDocument(writer, NULL, XML_ENCODING, NULL);
        xmlTextWriterStartElement(writer, BAD_CAST "Dictionary");
            // FileMap
            xmlTextWriterStartElement(writer, BAD_CAST "Files");
                for (const auto& [filename, element] : *m_file_map_ptr)
                {
                    xmlTextWriterStartElement(writer, BAD_CAST "File");
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST filename.c_str());
                        for (const auto& [term, freq] : element)
                        {
                            xmlTextWriterStartElement(writer, BAD_CAST "Term");
                                xmlTextWriterWriteAttribute(writer, BAD_CAST "key", BAD_CAST term.c_str());
                                xmlTextWriterWriteFormatString(writer, "%lu", freq);
                            xmlTextWriterEndElement(writer);
                        }
                    xmlTextWriterEndElement(writer);
                }
            xmlTextWriterEndElement(writer);

            // TermOccurrenceMap
            xmlTextWriterStartElement(writer, BAD_CAST "TermOccurrence");
                for (const auto& [term, occurrence] : *m_term_occurrence_map_ptr)
                {
                    xmlTextWriterStartElement(writer, BAD_CAST "Term");
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "key", BAD_CAST term.c_str());
                        xmlTextWriterWriteFormatString(writer, "%lu", occurrence);
                    xmlTextWriterEndElement(writer);
                }
            xmlTextWriterEndElement(writer);
        xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);

    xmlFreeTextWriter(writer);
}

void
SearchEngine::Dictionary::read_from_xml(const std::string& filename)
{
    FileMapPtr map(new FileMap());

    xmlDocPtr doc = xmlParseFile(filename.c_str());
    xmlNodePtr root = xmlDocGetRootElement(doc);

    if (root == NULL)
    {
        std::cerr << "ERROR: Input file is empty\n";
        return;
    }

    if (strcmp((char*) root->name, "Dictionary") != 0)
    {
        std::cerr << "ERROR: Expected a `Dictionary` tag at the beginning of the input file\n";
        return;
    }

    root = root->children;
    if (strcmp((char*) root->name, "Files") != 0)
    {
        std::cerr << "ERROR: Expected a `Files` tag as the first tag in Dictionary tag\n";
        return;
    }

    if (root->next == NULL || strcmp((char*) root->next->name, "TermOccurrence") != 0)
    {
        std::cerr << "ERROR: Expected a `TermOccurrence` after `Files` tag\n";
        return;
    }

    // Files Tag
    {

        xmlNodePtr current_file = root->children;
        do
        {
            if (strcmp((char*) current_file->name, "File") != 0)
            {
                std::cerr << "ERROR: Expected a `File` tag inside the `Files` tag\n";
                return;
            }

            if (current_file->properties == nullptr
                || current_file->properties->children == nullptr
                || current_file->properties->children->content == nullptr
                || strcmp((char*) current_file->properties->name, "name") != 0)
            {
                std::cerr << "ERROR: Expected a `name` attribute on the `File`, without any other attributes\n";
                return;
            }

            TermFreqMap term_freq_map;
            xmlNodePtr current_term = current_file->children;
            do
            {
                if (strcmp((char *)current_term->name, "Term") != 0)
                {
                    std::cerr << "ERROR: Expected a `Term` tag inside the `File` tag\n";
                    return;
                }

                if (current_term->properties == nullptr
                    || current_term->properties->children == nullptr
                    || current_term->properties->children->content == nullptr
                    || strcmp((char *)current_term->properties->name, "key") != 0)
                {
                    std::cerr << "ERROR: Expected a `key` attribute on the `Term`, without any other attributes\n";
                    return;
                }

                if (current_term->children == nullptr || strcmp((char *)current_term->children->name, "text") != 0)
                {
                    std::cerr << "ERROR: Expected `text` in `Term`\n";
                    return;
                }

                term_freq_map.insert(
                {
                    std::string((char*) current_term->properties->children->content),
                    atoi((char*)current_term->children->content)
                });
            }
            while ((current_term = current_term->next) != nullptr);

            map->insert({ std::string((char*) current_file->properties->children->content), term_freq_map });
        }
        while ((current_file = current_file->next) != nullptr);
    }

    // TermOccurrence Tag
    root = root->next;
    {
        xmlNodePtr current_term = root->children;
        do
        {
            if (current_term == NULL || strcmp((char *)current_term->name, "Term") != 0)
            {
                std::cerr << "ERROR: Expected a `Term` tag inside the `TermOccurrence` tag\n";
                return;
            }

            if (current_term->properties == nullptr
                || current_term->properties->children == nullptr
                || current_term->properties->children->content == nullptr
                || strcmp((char *)current_term->properties->name, "key") != 0)
            {
                std::cerr << "ERROR: Expected a `key` attribute on the `Term`, without any other attributes\n";
                return;
            }

            if (current_term->children == nullptr || strcmp((char *)current_term->children->name, "text") != 0)
            {
                std::cerr << "ERROR: Expected `text` in `Term`\n";
                return;
            }

            m_term_occurrence_map_ptr->insert(
            {
                std::string((char*) current_term->properties->children->content),
                atoi((char*)current_term->children->content)
            });
        }
        while ((current_term = current_term->next) != nullptr);
    }

    xmlFreeDoc(doc);

    m_file_map_ptr = std::move(map);
}

float
SearchEngine::Dictionary::tf(const std::string& term, const std::string& filename)
const
{
    TermFreqMap& term_freq_map = m_file_map_ptr->at(filename);
    auto freq_in_file_opt = term_freq_map.find(term);
    std::size_t freq_in_file = freq_in_file_opt != term_freq_map.end() ? freq_in_file_opt->second : 0;

    std::size_t freq_acc = 0;
    for (auto current = term_freq_map.begin(); current != term_freq_map.end(); ++current)
    {
        freq_acc += current->second;
    }

    return static_cast<float>(freq_in_file) / freq_acc;
}

float
SearchEngine::Dictionary::idf(const std::string& term)
const
{
    std::size_t N = m_file_map_ptr->size();
    auto term_occurrence_opt = m_term_occurrence_map_ptr->find(term);
    std::size_t term_occurrence =
        term_occurrence_opt != m_term_occurrence_map_ptr->end()
            ? term_occurrence_opt->second
            : 0;

    return std::log10(static_cast<float>(N) / (term_occurrence != 0 ? term_occurrence : 1));
}

void
SearchEngine::Dictionary::print()
const noexcept
{
    for (const auto& [filename, element] : *m_file_map_ptr)
    {
        std::cout << "Filename = " << filename << "\n";
        for (const auto &[term, freq] : element)
        {
            std::cout << "\t[" << term << "] = " << freq << "\n";
        }
    }
}

void
SearchEngine::Dictionary::insert_file(File&& file)
{
    m_file_map_ptr->insert(file);
}

void
SearchEngine::Dictionary::increase_term_occurrence(const std::string& term)
{
    auto term_occurrence = m_term_occurrence_map_ptr->find(term);

    if (term_occurrence != m_term_occurrence_map_ptr->end())
    {
        (*term_occurrence).second += 1;
    }
    else
    {
        m_term_occurrence_map_ptr->insert({ term, 1 });
    }
}

void
SearchEngine::Dictionary::write_to(const std::string& output_filename)
const
{
    write_to_xml(output_filename);
}

void
SearchEngine::Dictionary::read_from(const std::string& filename)
{
    read_from_xml(filename);
}

float
SearchEngine::Dictionary::tf_idf(const std::string& filename, const std::list<std::string>& tokens)
const
{
    float tf_idf = 0.0f;
    for (const auto& token : tokens)
    {
        tf_idf += tf(token, filename) * idf(token);
    }
    return tf_idf;
}
