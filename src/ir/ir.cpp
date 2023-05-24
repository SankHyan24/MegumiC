#include <ir/ir.hpp>
#include <string>
#include <iostream>
#include <fstream>
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
    char binOpEngStr[21][5] = {
        "add",
        "sub",
        "mul",
        "div",
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

    std::string BinOp2EngString(BinOp op)
    {
        return binOpEngStr[int(op)];
    }

    void IRcode::generate()
    {
        this->_generate();
    }

    std::string IRcode::dump()
    {
        return dst + "\n";
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
        dst += Var + " = " + BinOp2EngString(op) + " " + LHS + ", " + RHS;
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
        dst += Ptr + " = getptr " + Var + (Index.empty() ? ", 0" : ", " + Index);
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

    void IRVoid::_generate()
    {
        dst = "";
    }

    void IRListWrapper::Generate()
    {
        for (auto &i : *irList)
            i->generate();
    }

    std::string IRListWrapper::toString()
    {
        std::string ret;
        for (auto &i : *irList)
            ret += i->dump();
        return ret;
    }

    void IRListWrapper::Dump()
    {
        std::cout << toString();
    }

    void IRListWrapper::Dump(std::string filename)
    {
        std::ofstream fout(filename);
        fout << toString();
        fout.close();
    }

    void IRListWrapper::Dump(std::ostream &os)
    {
        os << toString();
    }
}