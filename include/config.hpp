#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace MC::config
{
    enum class StartMode
    {
        MC,
        IR
    };
    enum class EndMode
    {
        IR,
        RV32
    };
    enum class OptMode
    {
        None,
        O1,
        O2,
        O3
    };
    class Config
    {
    public:
        std::string inputFile;
        std::string irOutputFile;
        std::string targetOutputFile;

        Config(int argc, const char *argv[]);
        ~Config();
        std::ostream &getTargetOutputFileStream();
        std::ostream &getirOutputFileStream();

    private:
        std::ofstream targetOutputFileStream;
        std::ofstream irOutputFileStream;
    };
}
