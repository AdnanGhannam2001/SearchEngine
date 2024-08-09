#include "../includes/tokenizer.hpp"

std::string SearchEngine::Tokenizer::s_language = DEFAULT_LANGUAGE;

SearchEngine::Tokenizer::Tokenizer(const std::string &content)
    : m_content(std::string_view(content)),
      m_current(0)
{
    m_stemmer_ptr = sb_stemmer_new(s_language.c_str(), NULL);
}

bool
SearchEngine::Tokenizer::is_digit(const char c)
    const noexcept
{
    return c >= '0' && c <= '9';
}

bool
SearchEngine::Tokenizer::is_alpha(const char c)
    const noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string
SearchEngine::Tokenizer::scan_number()
{
    auto start = m_current;
    auto current_char = m_content.begin() + m_current;
    while ((current_char = m_content.begin() + m_current) != m_content.end() && is_digit(*current_char))
    {
        ++m_current;
    }

    return std::string(m_content.substr(start, m_current - start));
}

std::string
SearchEngine::Tokenizer::scan_alpha_numeric()
{
    auto start = m_current;
    auto current_char = m_content.begin() + m_current;
    while ((current_char = m_content.begin() + m_current) != m_content.end()
        && (is_alpha(*current_char) || is_digit(*current_char) || *current_char == '_'))
    {
        ++m_current;
    }

    std::string ret = std::string(m_content.substr(start, m_current - start));
    std::transform(ret.begin(), ret.end(), ret.begin(), [](char& c)
    {
        return std::tolower(c);
    });

    ret = (char*) sb_stemmer_stem(m_stemmer_ptr, (sb_symbol*) ret.c_str(), ret.length());

    return ret;
}

std::pair<bool, std::optional<std::string>>
SearchEngine::Tokenizer::next_token()
{
    auto current_char = m_content.begin() + m_current;

    if (current_char == m_content.end())
    {
        return { true, std::nullopt };
    }

    if (is_digit(*current_char))
    {
        return { false, scan_number() };
    }
    else if (is_alpha(*current_char))
    {
        return { false, scan_alpha_numeric() };
    }

    ++m_current;
    return { false, std::nullopt };
}

SearchEngine::Dictionary::TermFreqMap
SearchEngine::Tokenizer::scan_terms_in_file()
{
    Dictionary::TermFreqMap term_freq_map;

    while (true)
    {
        auto [eof, token] = next_token();

        if (eof) break;
        if (token.has_value())
        {
            auto value_opt = term_freq_map.find(*token);
            term_freq_map.insert_or_assign(*token,
                value_opt != term_freq_map.end()
                    ? (*value_opt).second + 1
                    : 1);
        }
    }

    return term_freq_map;
}

std::list<std::string>
SearchEngine::Tokenizer::scan_text()
{
    std::list<std::string> tokens;

    while (true)
    {
        auto [eof, token] = next_token();

        if (eof) break;
        if (token.has_value()) tokens.push_back(*token);
    }

    return tokens;
}

SearchEngine::Tokenizer::~Tokenizer()
{
    sb_stemmer_delete(m_stemmer_ptr);
}