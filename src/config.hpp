#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace MC::config
{
    extern int mode;
    extern std::string inputfile;
    extern std::string outputfile;

    void parse_arg(int argc, const char *argv[]);
}
