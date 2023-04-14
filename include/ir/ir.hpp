#pragma once
#include <iostream>
#include <list>
#include <string>
#include <iostream>
#include <functional>

namespace MC::IR
{
    enum class BinOp
    {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        LE,
        GE,
        LT,
        GT,
        EQ,
        NE,
        AND,
        OR,
    };
    // overload << operation of BinOp
    std::ostream &operator<<(std::ostream &os, const BinOp &p);

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
    };

    using IRList = std::list<IR>;
}
