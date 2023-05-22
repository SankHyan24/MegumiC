#pragma once
/***
 * This file contains the context of variables, constants, and functions.
 */
#include <vector>
#include <string>
#include <unordered_map>
#include <stack>

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
        Context();
        int id;
        int get_id() { return ++id; }
        int get_last_id() { return id; }
        using SymbolTable = std::vector<std::unordered_map<std::string, VarInfo>>;
        using ConstTable = std::vector<std::unordered_map<std::string, ConstInfo>>;

        SymbolTable symbol_table = {{}};
        ConstTable const_table = {{}};
        ConstTable const_assign_table = {{}};
        std::stack<std::string> loop_label; // push while_start and while_end

        void insert_symbol(std::string name, VarInfo value);
        void insert_const(std::string name, ConstInfo value);
        void insert_const_assign(std::string name, ConstInfo value);

        VarInfo &find_symbol(std::string name);
        ConstInfo &find_const(std::string name);
        ConstInfo &find_const_assign(std::string name);

        void create_scope();
        void end_scope();

        void add_loop_label(std::string label);
        void pop_loop_label();
        std::string get_loop_label();

        bool is_global();

    private:
    };

}