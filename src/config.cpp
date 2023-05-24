#include <config.hpp>
namespace MC::config
{

    Config::Config(int argc, const char *argv[])
    {
        inputFile = "../test/hello.mc";
        irOutputFile = "../hello.ir";
    }
}