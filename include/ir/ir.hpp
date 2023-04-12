#pragma once
#include <iostream>
#include <list>
#include <string>

namespace MC::IR
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

    enum class OpCode
    {
    };
    class IR
    {
    public:
        int line, column; // position in source code
        OpCode op_code;
        std::string label;
        OpName op1, op2, op3, dest;
        std::list<IR>::iterator phi_block;
        IR(OpCode op_code, std::string label = "");
        void print(std::ostream &out = std::cerr, bool verbose = false) const;
    };

    using IRList = std::list<IR>;
}
