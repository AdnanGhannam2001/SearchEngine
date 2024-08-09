#ifndef SEARCH_ENGINE_TOKENIZER_HPP
#define SEARCH_ENGINE_TOKENIZER_HPP

#include "common.hpp"
#include "dictionary.hpp"
#include <string_view>
#include <optional>
#include <mutex>
#include "libstemmer.h"

#define DEFAULT_LANGUAGE "english"

namespace SearchEngine
{
    class Tokenizer
    {
        using Stemmer = struct sb_stemmer;
        using StemmerPtr = Stemmer*;

    private:
        static std::string s_language;

        std::string_view m_content;
        std::size_t m_current;
        StemmerPtr m_stemmer_ptr;
    #if MULTITHREADING
        std::mutex m_mutex;
    #endif // MULTITHREADING

    private:
        bool
        is_digit(const char c)
            const noexcept;

        bool
        is_alpha(const char c)
            const noexcept;

        std::string
        scan_number();

        std::string
        scan_alpha_numeric();

    public:
        Tokenizer(const std::string& content);
        ~Tokenizer();

        std::pair<bool, std::optional<std::string>>
        next_token();

        Dictionary::TermFreqMap
        scan_terms_in_file();

        std::list<std::string> scan_text();
    };
}

#endif // SEARCH_ENGINE_TOKENIZER_HPP