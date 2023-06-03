#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

namespace MC::config
{
    enum class FirstMode
    {
        Version, // -v
        Help,    // -h
        Compile  // -c

    }; // default -c
    enum class StartMode
    {
        MC, // -mc from MC
        IR  // -irc from IRCode. NOT IMPLEMENTED

    }; // default -fmc
    enum class EndMode
    {
        IR,   // -ir to IR
        RV32, // -s to RV32
        Bin   // -bin

    }; // default -s
    enum class OptMode
    {
        None, // -O0
        O1,   // -O1
        O2,   // -O2
        O3    // -O3
    };
    class Config
    {
    public:
        void parse(int argc, const char *argv[]);
        void openFileStream();
        Config(int argc, const char *argv[]);

        void closeFileStream();
        ~Config();
        std::ostream &getTargetOutputFileStream();
        std::ostream &getirOutputFileStream();
        std::string &getPreCode();
        std::string &getInputCode();
        bool configInfo(std::ostream &out = std::cout) const;

    private:
        bool correct_input{true};
        // Main Config
        FirstMode first_mode{FirstMode::Compile};
        StartMode start_mode{StartMode::MC};
        EndMode end_mode{EndMode::RV32};
        OptMode opt_mode{OptMode::None};
        // input file name
        std::string input_file{"../test/task0.c"};
        // output file name
        std::string ir_output_file{"../test/a.ir"};
        std::string target_output_file{"../test/a.s"};

        // precode
        bool if_use_precode{false};
        std::string precode_path{"../test/pre.c"};

        // after parse
        std::string pre_code;
        std::string input_code;

        std::ofstream targetOutputFileStream;
        std::ofstream irOutputFileStream;
    };
}
