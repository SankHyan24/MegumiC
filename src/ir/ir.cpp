#include <ir/ir.hpp>
#include <string>
namespace
{
    // mapping from BinOp:
    char binOpStr[21][3] = {
        "+",
        "-",
        "*",
        "/",
        "%",
        "<=",
        ">=",
        "<",
        ">",
        "==",
        "!=",
        "&&",
        "||",
        "!"};

    // mapping from VarType
    char varTypeStr[2][5] = {
        "*i32",
        "i32"};

    std::string getVarTypeStr(MC::IR::VarType type)
    {
        return varTypeStr[int(type)];
    }
}

namespace MC::IR
{
    std::ostream &operator<<(std::ostream &os, const BinOp &p)
    {
        os << binOpStr[int(p)];
        return os;
    }

    std::string BinOp2String(BinOp op)
    {
        return binOpStr[int(op)];
    }

    void IRcode::DumpIR()
    {
        this->_generate();
        std::cout << dst << std::endl;
    }

    void IRFuncDef::_generate()
    {
        dst = "fun " + funcName;
        dst += "(";
        for (auto &i : args)
        {
            dst += i.name;
            dst += ": ";
            dst += getVarTypeStr(i.type);
            if (&i != &args.back())
                dst += ", ";
        }
        dst += "): ";
        dst += getVarTypeStr(retType);
        dst += " {";
    }

    void IRFuncDefEnd::_generate()
    {
        dst = "}";
    }

    void IRLabel::_generate()
    {
        dst = labelName + ":";
    }

    void IRRet::_generate()
    {
        dst = "    ";
        dst += "ret " + retVar;
    }

    void IRAssignImm::_generate()
    {
        dst = "    ";
        dst += Var + " = " + std::to_string(Imm);
    }

    void IRAssignBinOp::_generate()
    {
        dst = "    ";
        dst += Var + " = " + LHS + " " + BinOp2String(op) + " " + RHS;
    }

    void IRAssignUnaryOp::_generate()
    {
        dst = "    ";
        dst += Var + " = " + (op == MC::IR::BinOp::ADD ? "" : BinOp2String(op)) + +" " + RHS;
    }

    void IRCall::_generate()
    {
        dst = "    ";
        dst += Var + " = call " + FuncName + "(";
        for (auto &i : Args)
        {
            dst += i;
            if (&i != &Args.back())
                dst += ", ";
        }
        dst += ")";
    }

    void IRAlloc::_generate()
    {
        dst = "    ";
        dst += Var + " = alloc " + getVarTypeStr(AllocType);
    }

    void IRJump::_generate()
    {
        dst = "    ";
        dst += "jump " + Label;
    }

    void IRBranch::_generate()
    {
        dst = "    ";
        dst += "br " + Cond + ", " + IfLabel + ", " + ElseLabel;
    }

    void IRGetPtr::_generate()
    {
        dst = "    ";
        dst += Ptr + " = getptr " + Var;
    }

    void IRGetElementPtr::_generate()
    {
        dst = "    ";
        dst += Ptr + " = getelemptr " + Arr + ", " + Ind;
    }

    void IRStore::_generate()
    {
        dst = "    ";
        dst += "store " + Value + ", " + AddressOfTarget;
    }

    void IRLoad::_generate()
    {
        dst = "    ";
        dst += Var + " = load " + Ptr;
    }

    void IRGlobalVar::_generate()
    {
        dst = "global ";
        dst += Var + " = alloc " + getVarTypeStr(AllocType) + ", " + InitList;
    }

    void IRArrayDef::_generate()
    {
        dst = "    ";
        dst += Var + " = alloc ";
        VarType type = MC::IR::VarType::Val;
        for (int i = 0; i < Shape.size(); i++)
            dst += "[";
        dst += "i32";
        for (int i = Shape.size() - 1; i >= 0; i--)
            dst += ", " + std::to_string(Shape[i]) + "]";
    }

    void IRGlobalArray::_generate()
    {
        dst = "global ";
        dst += Var + " = alloc ";
        VarType type = MC::IR::VarType::Val;
        for (int i = 0; i < Shape.size(); i++)
            dst += "[";
        dst += "i32";
        for (int i = Shape.size() - 1; i >= 0; i--)
            dst += ", " + std::to_string(Shape[i]) + "]";
        dst += ", " + InitList;
    }
}