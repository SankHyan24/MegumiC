#pragma once
#include <iostream>
#include <list>
#include <string>
#include <iostream>
#include <functional>
#include <memory>

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
        NOT // 虽然not不是二元操作符，但是我懒得再写一个了
    };

    enum class IROp
    {
        FuncDef,       // fun @half(@x: i32): i32 { done
        FuncDefEnd,    // done
        Label,         // %entry:
        Call,          // done
        AssignBinOp,   // done
        AssignUnaryOp, // done
        AssignImm,     // done
        Jump,
        Branch,
        GetPtr,
        GetElementPtr,
        ArrayDef,
        Store,
        Load,
        Alloc, // todo
        Ret,   // done
        FunctionCall,
        GlobalVar,
        GlobalArray
    };

    enum class VarType
    {
        Ptr,
        Val
    };

    class ArgPair
    {
    public:
        VarType type;
        std::string name;
        ArgPair(VarType type, std::string name) : type(type), name(name) {}
    };

    // overload << operation of BinOp
    std::ostream &operator<<(std::ostream &os, const BinOp &p);

    class IRcode
    {
    public:
        IROp IRType;
        std::string dst;
        virtual ~IRcode() = default;

        void DumpIR();

    private:
        virtual void _generate() { std::cout << "cnd"; };
    };

    class IRFuncDef : public IRcode
    {
    public:
        std::string funcName;
        VarType retType;
        std::vector<ArgPair> args;
        IRFuncDef(std::string funcName, VarType retType, std::vector<ArgPair> args) : funcName(funcName), retType(retType), args(args) {}

    private:
        virtual void _generate() override;
    };

    class IRFuncDefEnd : public IRcode
    {
    public:
        IRFuncDefEnd(){};

    private:
        virtual void _generate() override;
    };

    class IRLabel : public IRcode
    {
    public:
        std::string labelName;
        IRLabel(std::string labelName) : labelName(labelName) {}

    private:
        virtual void _generate() override;
    };

    class IRRet : public IRcode
    {
    public:
        std::string retVar;
        IRRet(std::string retVar) : retVar(retVar) {}

    private:
        virtual void _generate() override;
    };

    class IRAssignImm : public IRcode
    {
    public:
        std::string Var;
        int Imm;
        IRAssignImm(std::string Var, int Imm) : Var(Var), Imm(Imm) {}

    private:
        virtual void _generate() override;
    };

    class IRAssignBinOp : public IRcode
    {
    public:
        std::string Var;
        std::string LHS;
        std::string RHS;
        BinOp op;
        IRAssignBinOp(std::string Var, std::string LHS, std::string RHS, BinOp op) : Var(Var), LHS(LHS), RHS(RHS), op(op) {}

    private:
        virtual void _generate() override;
    };

    class IRAssignUnaryOp : public IRcode
    {
    public:
        std::string Var;
        std::string RHS;
        BinOp op;
        IRAssignUnaryOp(std::string Var, std::string RHS, BinOp op) : Var(Var), RHS(RHS), op(op) {}

    private:
        virtual void _generate() override;
    };

    class IRCall : public IRcode
    {
    public:
        std::string Var;
        std::string FuncName;
        std::vector<std::string> Args;
        IRCall(std::string Var, std::string FuncName, std::vector<std::string> Args) : Var(Var), Args(Args), FuncName(FuncName) {}

    private:
        virtual void _generate() override;
    };
    class IRAlloc : public IRcode
    {
    public:
        std::string Var;
        VarType AllocType;
        IRAlloc(std::string Var, VarType AllocType) : Var(Var), AllocType(AllocType) {}

    private:
        virtual void _generate() override;
    };
    using IRList = std::list<std::unique_ptr<IRcode>>;
}
