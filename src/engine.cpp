#include "../includes/engine.hpp"

void
SearchEngine::Engine::extract_from_file(const std::string& filename)
{
    XmlParser parser(filename);
    std::string file_content = parser.parse();

    Tokenizer tokenizer(file_content);

    auto term_freq_map = tokenizer.scan_terms_in_file();

    for (const auto& term_freq : term_freq_map)
    {
#if MULTITHREADING
        std::lock_guard<std::mutex> guard(m_mutex);
#endif // MULTITHREADING
        m_dictionary.increase_term_occurrence(term_freq.first);
    }

    m_dictionary.insert_file({ filename, std::move(term_freq_map) });
}

std::list<std::string>
SearchEngine::Engine::get_files_from_dir(const std::string& dirname)
{
    std::list<std::string> files;

    for (const auto &entry : std::filesystem::directory_iterator(dirname))
    {
        if (entry.is_directory())
        {
            std::list<std::string> files_in_dir = get_files_from_dir(entry.path());
            files.insert(files.end(), files_in_dir.begin(), files_in_dir.end());
        }
        else if (entry.path().extension() == FILE_EXTENSION)
        {
            files.push_back(entry.path());
        }
    }

    return files;
}

int
SearchEngine::Engine::index(const std::string& dirname, const std::string& out_filename)
{
    std::list<std::string> filesnames = get_files_from_dir(dirname);

#if MULTITHREADING
    std::list<std::thread> threads;

    for (auto& filename : filesnames)
    {
        std::cout << "Indexing: '" << filename << "'\n";
        threads.push_back(
            std::thread(
                &SearchEngine::Engine::extract_from_file,
                this,
                std::cref(filename)));

        if (threads.size() >= MAX_THREADS)
        {
            for (std::thread& thread : threads)
            {
                thread.join();
            }

            threads.clear();
        }
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }
#else
    for (auto& filename : filesnames)
    {
        std::cout << "Indexing: '" << filename << "'\n";
        extract_from_file(filename);
    }
#endif // MULTITHREADING

    std::cout << "Writing to file...\n";
    m_dictionary.write_to(out_filename);

    return 0;
}

void
SearchEngine::Engine::calculate_tf_idf_result(const std::string& filename,
    const std::list<std::string>& tokens,
    std::list<std::pair<std::string, float>>& results)
{
    float tf_idf = m_dictionary.tf_idf(filename, tokens);

    if (tf_idf > EP)
    {
#if MULTITHREADING
        std::lock_guard<std::mutex> guard(m_mutex);
#endif // MULTITHREADING
        results.push_back({ filename, tf_idf });
    }
}

int 
SearchEngine::Engine::search(const std::string& index)
{
    std::cout << "Loading dictionary file...\n";
    m_dictionary.read_from(index);

    std::cout << "> ";
    std::string query;
    while (std::getline(std::cin, query))
    {
        Tokenizer query_tokenizer(query);
        std::list<std::string> tokens = query_tokenizer.scan_text();
        std::list<std::pair<std::string, float>> results;

        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& [filename, _] : *m_dictionary.m_file_map_ptr)
        {
            calculate_tf_idf_result(filename, tokens, results);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << results.size()
            << " result found in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms\n";

        results.sort(
            [](std::pair<std::string, float> a, std::pair<std::string, float> b)
            {
                return a.second > b.second;
            });

        for (const auto& result : results)
        {
            std::cout << "[" << result.first << "] = " << result.second << "\n";
        }

        std::cout << "> ";
    }

    return 0;
}

void
SearchEngine::Engine::usage()
const noexcept
{
    std::cout << "Usage: se COMMAND <Args...>\n";
    std::cout << "Commands:\n";
    std::cout << "\tindex  <input_file> <output_file> Index the input file and save to the output file.\n";
    std::cout << "\tsearch <index_file>               Perform a search using an indexed file.\n";
}

int
SearchEngine::Engine::start(int argc, char** argv)
{
    LIBXML_TEST_VERSION;

    if (argc == 4 && strcmp(argv[1], "index") == 0)
    {
        return index(argv[2], argv[3]);
    }
    else if (argc == 3 && strcmp(argv[1], "search") == 0)
    {
        return search(argv[2]);
    }

    usage();
    return 1;
}