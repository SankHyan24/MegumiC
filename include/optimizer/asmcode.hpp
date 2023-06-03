#pragma once
#include <memory>
#include <sstream>
#include <vector>

#include <ir/ir.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>

namespace MC::ASM
{
    enum class AsmOp
    {
        ASM,   // .xxx
        LABEL, // xxx:
        //
        CALL,
        STORE,
        LOAD,
        //
        LUI,
        BNE,
        JMP,
        JR,
        LA,
        LI,
        // special
        BINOP,
        //
        ADDI,
        ADD,
        SUB,
        MUL,
        DIV,
        REM,
        SLT,
        AND,
        ANDI,
        SGT,
        XOR,
        XORI,
        SEQZ,
        SNEZ,
        OR,
        ORI,

    };

    class AsmCode
    {
    public:
        AsmOp op;
        std::string opstr;
        std::string arg1;
        std::string arg2;
        std::string arg3;
        std::string comment;
        AsmCode(AsmOp op, std::string arg1, std::string arg2, std::string arg3, std::string comment = "") : op(op), arg1(arg1), arg2(arg2), arg3(arg3), comment(comment){};
        std::string getString();
    };
    class AsmFile
    {
    public:
        std::vector<AsmCode> codes;
        void Generate();
        void Dump(std::ostream &out = std::cout);
        AsmFile(std::string asm_code);

    private:
    };
}