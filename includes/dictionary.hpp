#ifndef TERMS_FREQUENCY_MAP_HPP
#define TERMS_FREQUENCY_MAP_HPP

#include "common.hpp"
#include <cstdlib>
#include <cmath>
#include <libxml/xmlwriter.h>

#define XML_ENCODING "UTF-8"

namespace SearchEngine
{
    class Dictionary
    {
    public:
        using TermFreq = std::pair<std::string, std::size_t>;
        using TermFreqMap = std::unordered_map<std::string, std::size_t>;

        using File = std::pair<std::string, TermFreqMap>;
        using FileMap = std::unordered_map<std::string, TermFreqMap>;
        using FileMapPtr = std::unique_ptr<FileMap>;

        using TermOccurrence = std::pair<std::string, std::size_t>;
        using TermOccurrenceMap = std::unordered_map<std::string, std::size_t>;
        using TermOccurrenceMapPtr = std::unique_ptr<TermOccurrenceMap>;

    private:
        FileMapPtr m_file_map_ptr;
        TermOccurrenceMapPtr m_term_occurrence_map_ptr;
        
    private:
        void
        write_to_xml(const std::string& output_filename)
        const;

        void
        read_from_xml(const std::string& filename);

        float
        tf(const std::string& term, const std::string& filename)
        const;

        float
        idf(const std::string& term)
        const;

    public:
        Dictionary();

        void
        print()
        const noexcept;

        void
        insert_file(File&& file);

        void
        increase_term_occurrence(const std::string&);

        void
        write_to(const std::string& output_filename)
        const;

        void
        read_from(const std::string& filename);

        float
        tf_idf(const std::string& filename, const std::list<std::string>& tokens)
        const;

        friend class Engine;
    };
}

#endif // TERMS_FREQUENCY_MAP_HPP
