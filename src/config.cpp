#include <config.hpp>
namespace MC::config
{

    Config::Config(int argc, const char *argv[])
    {
        inputFile = "../test/hello.mc";
        irOutputFile = "../zriscv/hello.ir";
        targetOutputFile = "../zriscv/target.s";
        irOutputFileStream.open(irOutputFile);
        targetOutputFileStream.open(targetOutputFile);
    }
    Config::~Config()
    {
        targetOutputFileStream.close();
        irOutputFileStream.close();
    }

    std::ostream &Config::getTargetOutputFileStream()
    {
        return targetOutputFileStream;
    }
    std::ostream &Config::getirOutputFileStream()
    {
        return irOutputFileStream;
    }

}