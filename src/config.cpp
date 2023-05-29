#include <config.hpp>
#include <iostream>
namespace MC::config::color
{
    std::string RED("\033[0;31m");
    std::string GREEN("\033[0;32m");
    std::string YELLOW("\033[0;33m");
    std::string BLUE("\033[0;34m");
    std::string PURPLE("\033[0;35m");
    std::string CYAN("\033[0;36m");
    std::string WHITE("\033[0;37m");
    std::string RESET("\033[0m");
}

namespace MC::config::util
{
    std::string FirstMode2Str(MC::config::FirstMode mode)
    {
        switch (mode)
        {
        case MC::config::FirstMode::Version:
            return "Version";
        case MC::config::FirstMode::Help:
            return "Help";
        case MC::config::FirstMode::Compile:
            return "Compile";
        default:
            return "Unknown";
        }
    }

    std::string StartMode2Str(MC::config::StartMode mode)
    {
        switch (mode)
        {
        case MC::config::StartMode::MC:
            return "MC";
        case MC::config::StartMode::IR:
            return "IR";
        default:
            return "Unknown";
        }
    }

    std::string EndMode2Str(MC::config::EndMode mode)
    {
        switch (mode)
        {
        case MC::config::EndMode::IR:
            return "IR";
        case MC::config::EndMode::RV32:
            return "RV32";
        case MC::config::EndMode::Bin:
            return "Bin";
        default:
            return "Unknown";
        }
    }

    std::string OptMode2Str(MC::config::OptMode mode)
    {
        switch (mode)
        {
        case MC::config::OptMode::None:
            return "None";
        case MC::config::OptMode::O1:
            return "O1";
        case MC::config::OptMode::O2:
            return "O2";
        case MC::config::OptMode::O3:
            return "O3";
        default:
            return "Unknown";
        }
    }
}

namespace MC::config
{

    Config::Config(int argc, const char *argv[])
    {
        parse(argc, argv);
        openFileStream();
        if (!correct_input)
            first_mode = FirstMode::Help;
    }
    void Config::parse(int argc, const char *argv[])
    {
        // parse argv to args
        std::vector<std::pair<bool, std::string>> args;
        for (int i = 1; i < argc; i++)
            argv[i][0] == '-' ? args.push_back(std::make_pair(true, argv[i])) : args.push_back(std::make_pair(false, argv[i]));
        std::vector<std::string> options;
        std::vector<std::pair<std::string, std::string>> option_pairs;
        for (int i = 0; i < args.size(); i++)
        {
            if (args[i].first)
            {
                if (i + 1 < args.size() && !args[i + 1].first)
                {
                    option_pairs.push_back(std::make_pair(args[i].second, args[i + 1].second));
                    i++;
                }
                else
                    options.push_back(args[i].second);
            }
            else
            {
                if (i != 0)
                {
                    correct_input = false;
                    std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << args[i].second << color::RESET << "]" << std::endl;
                    std::cout << "  You should not put arg " << color::BLUE << args[i].second << color::RESET << " at this place." << std::endl;
                    std::cout << "  You should put it after an option, like '-mc'" << std::endl;
                    return;
                }
                options.push_back(args[i].second);
            }
        }
        // parse options
        bool first_mode_set{false};
        bool start_mode_set{false};
        bool end_mode_set{false};
        bool opt_mode_set{false};
        bool first_option{true};
        for (auto &option : options)
        {
            if (!(option.compare("-v") && option.compare("-h") && option.compare("-c")))
            {
                first_option = false;
                if (first_mode_set)
                {
                    correct_input = false;
                    std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                    std::cout << "  You have set mode as" << color::BLUE << util::FirstMode2Str(first_mode) << color::RESET << std::endl;
                    return;
                }
                first_mode = option.compare("-v") ? (option.compare("-h") ? FirstMode::Compile : FirstMode::Help) : FirstMode::Version;
                first_mode_set = true;
                continue;
            }
            if (!(option.compare("-O1") && option.compare("-O2") && option.compare("-O3")))
            {
                first_option = false;
                if (opt_mode_set)
                {
                    correct_input = false;
                    std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                    std::cout << "  You have set Optmode as" << color::BLUE << util::OptMode2Str(opt_mode) << color::RESET << std::endl;
                    return;
                }
                opt_mode = option.compare("-O1") ? (option.compare("-O2") ? OptMode::O3 : OptMode::O2) : OptMode::O1;
                opt_mode_set = true;
                continue;
            }
            if (first_option)
            {
                correct_input = true;
                start_mode_set = true;
                end_mode_set = true;
                first_option = false;
                input_file = option;
            }
            else
            {
                correct_input = false;
                std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                std::cout << "  Unknown option " << color::BLUE << option << color::RESET << std::endl;
                return;
            }
        }
        for (auto &option_pair : option_pairs)
        {
            std::string option = option_pair.first;
            std::string value = option_pair.second;
            if (!(option.compare("-mc") || option.compare("-irc")))
            {
                if (start_mode_set)
                {
                    correct_input = false;
                    std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                    std::cout << "  You have set start mode as" << color::BLUE << util::StartMode2Str(start_mode) << color::RESET << std::endl;
                    return;
                }
                start_mode = option.compare("-mc") ? StartMode::IR : StartMode::MC;
                start_mode_set = true;
                continue;
            }
            if (!(option.compare("-ir") || option.compare("-s") || option.compare("-bin")))
            {
                if (end_mode_set)
                {
                    correct_input = false;
                    std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                    std::cout << "  You have set end mode as" << color::BLUE << util::EndMode2Str(end_mode) << color::RESET << std::endl;
                    return;
                }
                end_mode = option.compare("-ir") ? (option.compare("-s") ? EndMode::Bin : EndMode::RV32) : EndMode::IR;
                end_mode_set = true;
            }
            else
            {
                correct_input = false;
                std::cout << color::RED << "Wrong input args" << color::RESET << ": [" << color::CYAN << option << color::RESET << "]" << std::endl;
                std::cout << "  Unknown option " << color::BLUE << option << color::RESET << std::endl;
                std::cout << "  Or option " << color::BLUE << option << color::RESET << " cannot be followed by a value" << std::endl;
                return;
            }
        }
    }
    void Config::openFileStream()
    {
        irOutputFileStream.open(ir_output_file);
        targetOutputFileStream.open(target_output_file);
    }
    void Config::closeFileStream()
    {
        irOutputFileStream.close();
        targetOutputFileStream.close();
    }

    Config::~Config()
    {
        closeFileStream();
    }
    bool Config::configInfo(std::ostream &out) const
    {
        std::string help("");
        help = help + "Usage: MC [options] <input file>\nOptions:\n" +
               "  -v\t\t\tPrint version\n" +
               "  -h\t\t\tPrint help\n" +
               "  -c\t\t\tCompile\n" +
               "  -mc\t\t\tCompile from MC. \tFollowed by the input file path.\n" +
               "  -irc\t\t\tCompile from IR. \tFollowed by the input file path.\n" +
               "  -ir\t\t\tOutput IR code. \tFollowed by the output file path.\n" +
               "  -s\t\t\tOutput RV32 code. \tFollowed by the output file path.\n" +
               "  -bin\t\t\tOutput binary code. \tFollowed by the output file path.\n" +
               "  -O0\t\t\tNo optimization\n" +
               "  -O1\t\t\tOptimization level 1\n" +
               "  -O2\t\t\tOptimization level 2\n" +
               "  -O3\t\t\tOptimization level 3\n" +
               "  -pre\t\t\tUse precode. \tFollowed by the precode file path.\n";
        if (first_mode == MC::config::FirstMode::Version)
            out << "Version: 0.0.1";
        else if (first_mode == MC::config::FirstMode::Help)
            out << "Help:\n"
                << help;
        return correct_input;
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
        // read from input_file
        std::string res("");
        std::ifstream input_file_stream(input_file);
        if (input_file_stream.is_open())
        {
            std::string line;
            while (getline(input_file_stream, line))
                res += line + "\n";
            input_file_stream.close();
        }
        else
            std::cout << "No Input file" << std::endl;
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