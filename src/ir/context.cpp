#include <ir/context.hpp>
#include <stdexcept>

namespace MC::IR
{
    VarInfo::VarInfo(std::string name, bool is_array, std::vector<int> shape)
        : shape(shape), is_array(is_array), name(name) {}

    Context::Context()
    {
        id = 0;
    }

    void Context::insert_symbol(std::string name, VarInfo value)
    {
        symbol_table.back().insert({name, value});
    }

    void Context::insert_const(std::string name, ConstInfo value)
    {
        const_table.back().insert({name, value});
    }

    void Context::insert_const_assign(std::string name, ConstInfo value)
    {
        const_assign_table.back().insert({name, value});
    }

    VarInfo &Context::find_symbol(std::string name)
    {
        for (int i = symbol_table.size() - 1; i >= 0; i--)
        {
            auto find = symbol_table[i].find(name);
            if (find != symbol_table[i].end())
                return find->second;
        }
        throw std::out_of_range("No such symbol:" + name);
    }

    ConstInfo &Context::find_const(std::string name)
    {
        for (int i = const_table.size() - 1; i >= 0; i--)
        {
            auto find = const_table[i].find(name);
            if (find != const_table[i].end())
                return find->second;
        }
        throw std::out_of_range("No such const:" + name);
    }

    ConstInfo &Context::find_const_assign(std::string name)
    {
        for (int i = const_assign_table.size() - 1; i >= 0; i--)
        {
            auto find = const_assign_table[i].find(name);
            if (find != const_assign_table[i].end())
                return find->second;
        }
        throw std::out_of_range("No such const:" + name);
    }

    void Context::create_scope()
    {
        symbol_table.push_back({});
        const_table.push_back({});
        const_assign_table.push_back({});
    }

    void Context::end_scope()
    {
        symbol_table.pop_back();
        const_table.pop_back();
        const_assign_table.pop_back();
    }
}