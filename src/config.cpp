#include <config.hpp>
namespace MC::config
{
    int mode;
    std::string inputfile;
    std::string outputfile;

    void parse_arg(int argc, const char *argv[])
    {
        mode = 0;
        inputfile = "../test/hello.mc";
        outputfile = "../hello.out";
        // for (int i = 1; i < argc; i++) {}
    }
}