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
    char irOPEngStr[19][20] = {
        "FuncDef",
        "FuncDefEnd",
        "Label",
        "Call",
        "AssignBinOp",
        "AssignUnaryOp",
        "AssignImm",
        "Jump",
        "Branch",
        "GetPtr",
        "GetElementPtr",
        "ArrayDef",
        " Store",
        "Load",
        "Alloc",
        "Ret",
        "GlobalVar",
        "GlobalArray",
        "Void"};

    // mapping from VarType
    char varTypeStr[2][5] = {
        "*i32",
        "i32"};

    char binOpRV32Str[21][5] = {
        "add",
        "sub",
        "mul",
        "div",
        "rem",
        "sle",
        "sge",
        "slt",
        "sgt",
        "seq",
        "sne",
        "and",
        "or",
        "not"};

    std::string getVarTypeStr(MC::IR::VarType type)
    {
        return varTypeStr[int(type)];
    }

}

namespace MC::IR
{
    std::string IROp2String(IROp op)
    {
        return irOPEngStr[int(op)];
    }
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

    std::string BinOp2RV32String(BinOp op)
    {
        return binOpRV32Str[int(op)];
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
        IRType = MC::IR::IROp::FuncDef;
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
        IRType = MC::IR::IROp::FuncDefEnd;
        dst = "}";
    }

    void IRLabel::_generate()
    {
        IRType = MC::IR::IROp::Label;
        dst = labelName + ":";
    }

    void IRRet::_generate()
    {
        IRType = MC::IR::IROp::Ret;
        dst = "    ";
        dst += "ret " + retVar;
    }

    void IRAssignImm::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::AssignImm;
        dst = "    ";
        dst += Var + " = " + std::to_string(Imm);
    }

    void IRAssignBinOp::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::AssignBinOp;
        dst = "    ";
        dst += Var + " = " + BinOp2EngString(op) + " " + LHS + ", " + RHS;
    }

    void IRAssignUnaryOp::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::AssignUnaryOp;
        dst = "    ";
        dst += Var + " = " + (op == MC::IR::BinOp::ADD ? "" : BinOp2String(op)) + +" " + RHS;
    }

    void IRCall::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::Call;
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
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::Alloc;
        dst = "    ";
        dst += Var + " = alloc " + getVarTypeStr(AllocType);
    }

    void IRJump::_generate()
    {
        IRType = MC::IR::IROp::Jump;
        dst = "    ";
        dst += "jump " + Label;
    }

    void IRBranch::_generate()
    {
        IRType = MC::IR::IROp::Branch;
        dst = "    ";
        dst += "br " + Cond + ", " + IfLabel + ", " + ElseLabel;
    }

    void IRGetPtr::_generate()
    {
        if_new_var = true;
        new_var_name = Ptr;
        IRType = MC::IR::IROp::GetPtr;
        dst = "    ";
        dst += Ptr + " = getptr " + Var + (Index.empty() ? ", 0" : ", " + Index);
    }

    void IRGetElementPtr::_generate()
    {
        if_new_var = true;
        new_var_name = Ptr;
        IRType = MC::IR::IROp::GetElementPtr;
        dst = "    ";
        dst += Ptr + " = getelemptr " + Arr + ", " + Ind;
        dst += " // ";
        dst += "类型：";
        dst += ((IRGetElementPtrType == 0) ? "第一次" : "多次的");
        dst += " 层数: " + std::to_string(Lvl);
    }

    void IRStore::_generate()
    {
        IRType = MC::IR::IROp::Store;
        dst = "    ";
        dst += "store " + Value + ", " + AddressOfTarget;
    }

    void IRLoad::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::Load;
        dst = "    ";
        dst += Var + " = load " + Ptr;
    }

    void IRGlobalVar::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::GlobalVar;
        dst = "global ";
        dst += Var + " = alloc " + getVarTypeStr(AllocType) + ", " + InitList;
    }

    void IRArrayDef::_generate()
    {
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::ArrayDef;
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
        if_new_var = true;
        new_var_name = Var;
        IRType = MC::IR::IROp::GlobalArray;
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
        IRType = MC::IR::IROp::Void;
        dst = "";
    }

    void IRListWrapper::Generate()
    {
        for (auto &i : *irList)
        {
            i->generate();
        }
    }

    std::string IRListWrapper::toString()
    {
        std::string ret;
        if (this->print_ir_line_number)
            for (int i = 0; i < irList->size(); i++)
                ret += "[" + std::to_string(i) + "]\t" + (*irList)[i]->dump();
        else
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