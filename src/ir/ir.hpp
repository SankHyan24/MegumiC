#pragma once
#include <functional>
#include <iostream>
#include <list>
#include <string>

namespace MC::ir
{
    class OpName
    {
    protected:
        enum Type
        {
            Var,
            Imm,
            Null,
        };

    public:
        Type type;
        std::string name;
        int value;
        OpName();
        OpName(std::string name);
        OpName(int value);
        bool is_var() const;
        bool is_local_var() const;
        bool is_global_var() const;
        bool is_imm() const;
        bool is_null() const;
        bool operator==(const OpName &other) const;
    };
    class IR
    {
    };
}
