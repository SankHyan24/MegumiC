#pragma once
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>

#include <config.hpp>
#include <ir/ir.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>

namespace MC::OPT
{
    enum class AsmOp
    {
        COMMENT,
        ASM,   // .xxx
        LABEL, // xxx:
        CALL,
        // special
        L_S,
        // STORE,
        // LOAD,
        //
        LUI,
        BNE,
        //
        JMP,
        JR,
        LA,
        LI,
        // special
        BINOP,
        //
        // ADDI,
        // ADD,
        // SUB,
        // MUL,
        // DIV,
        // REM,
        // SLT,
        // AND,
        // ANDI,
        // SGT,
        // XOR,
        // XORI,
        // SEQZ,
        // SNEZ,
        // OR,
        // ORI,

    };

    class AsmCode
    {
    public:
        AsmCode(AsmOp op, std::string arg1, std::string arg2, std::string arg3, std::string comment = "") : op(op), arg1(arg1), arg2(arg2), arg3(arg3), comment(comment){};
        AsmCode(std::string target_inst_str);
        std::string getString();
        void Dump(std::ostream &out = std::cout);

        AsmOp getOp() const { return this->op; }
        std::string getOpStr() const { return this->opstr; }
        std::string getArg1() const { return this->arg1; }
        std::string getArg2() const { return this->arg2; }
        std::string getArg3() const { return this->arg3; }

    private:
        AsmOp op;
        // xxx:
        std::string label;
        // .xxx (yyy)
        std::string asmstr;
        std::string asmstr_arg;
        // opstr arg1, arg2, arg3
        std::string opstr;
        std::string arg1;
        std::string arg2;
        std::string arg3;
        // comment
        std::string comment;
    };

}