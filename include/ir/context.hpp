#pragma once
/***
 * This file contains the context of variables, constants, and functions.
 */
#include <vector>
#include <string>

namespace MC::IR
{
    class VarInfo
    {
    public:
        std::vector<int> shape;
        bool is_array;
        std::string name; // with @$%
        VarInfo(std::string name, bool is_array = false, std::vector<int> shape = {});
    };

    class ConstInfo
    {
    public:
        bool is_array;
        std::vector<int> shape;
        std::vector<int> value;
        ConstInfo(std::vector<int> value, bool is_array = false,
                  std::vector<int> shape = {});
        ConstInfo(int value);
    };

    // class FuncInfo
    // {
    // public:
    //     std::string name;
    //     std::vector<VarInfo> args;
    //     std::vector<VarInfo> rets;
    //     FuncInfo(std::string name, std::vector<VarInfo> args,
    //              std::vector<VarInfo> rets);
    // };

    class Context
    {
    public:
        unsigned int id;
        unsigned int get_id() { return ++id; }

        Context();

    private:
    };

}