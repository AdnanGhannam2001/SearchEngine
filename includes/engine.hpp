#ifndef SEARCH_ENGINE_ENGINE_HPP
#define SEARCH_ENGINE_ENGINE_HPP

#include <thread>
#include <mutex>
#include <filesystem>
#include "common.hpp"
#include "tokenizer.hpp"
#include "xml-parser.hpp"
#include "xml-parser.hpp"

#define FILE_EXTENSION ".html"
#define EP 1.0e-03f
#define MAX_THREADS 250 

namespace SearchEngine
{
    class Engine
    {
    private:
        Dictionary m_dictionary;
    #if MULTITHREADING
        std::mutex m_mutex;
    #endif // MULTITHREADING

    private:
        void
        extract_from_file(const std::string &filename);

        std::list<std::string>
        get_files_from_dir(const std::string&);

        void
        calculate_tf_idf_result(const std::string& filename,
            const std::list<std::string>& tokens,
            std::list<std::pair<std::string, float>>& results);

        void
        usage()
        const noexcept;

        int
        index(const std::string& dirname, const std::string& out_filename);

        int
        search(const std::string& index);


    public:
        int
        start(int, char**);
    };
}

#endif // SEARCH_ENGINE_ENGINE_HPP