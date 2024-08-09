#include "includes/engine.hpp"

int main(int argc, char **argv)
{
    SearchEngine::Engine engine;
    int ret = engine.start(argc, argv);

    return ret;
}
