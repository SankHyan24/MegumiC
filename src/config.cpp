#include <config.hpp>
namespace MC::config
{

    Config::Config(int argc, const char *argv[])
    {
        inputFile = "../test/task4.c";
        irOutputFile = "../zriscv/hello.ir";
        targetOutputFile = "../zriscv/target.s";
        irOutputFileStream.open(irOutputFile);
        targetOutputFileStream.open(targetOutputFile);
        // if ./mc 1
        // use pre code
        if (argc == 2 && argv[1][0] == '1')
            if_use_precode = true;
    }
    Config::~Config()
    {
        targetOutputFileStream.close();
        irOutputFileStream.close();
    }

    std::string &Config::getPreCode()
    {
        // read from precode_path
        std::string res("");
        std::ifstream precode_file(precode_path);
        if (precode_file.is_open())
        {
            std::string line;
            while (getline(precode_file, line))
                res += line + "\n";
            precode_file.close();
        }
        else
            std::cout << "No Precode file";
        pre_code = res;
        return pre_code;
    }
    std::string &Config::getInputCode()
    {
        // read from inputFile
        std::string res("");
        std::ifstream input_file(inputFile);
        if (input_file.is_open())
        {
            std::string line;
            while (getline(input_file, line))
                res += line + "\n";
            input_file.close();
        }
        else
            std::cout << "No Input file";
        if (if_use_precode)
            res = getPreCode() + res;
        input_code = res;
        return input_code;
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