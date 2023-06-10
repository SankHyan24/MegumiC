#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <memory>

#include <config.hpp>

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
        Jump,          // done
        Branch,        // done
        GetPtr,        // done
        GetElementPtr, // done
        ArrayDef,      // done
        Store,         // done
        Load,          // done
        Alloc,         // done
        Ret,           // done
        GlobalVar,     // done
        GlobalArray,   // done
        Void           // done
    };

    enum class VarType
    {
        Ptr,
        Val,
        Void
    };

    class ArgPair
    {
    public:
        VarType type;
        std::string name;
        ArgPair(VarType type, std::string name) : type(type), name(name) {}
    };

    // overload << operation of BinOp
    std::string IROp2String(IROp op);
    std::ostream &operator<<(std::ostream &os, const BinOp &p);
    std::string BinOp2EngString(BinOp op);
    std::string BinOp2RV32String(BinOp op);

    class IRcode
    {
    public:
        IROp IRType{IROp::Void};
        bool if_new_var{false};
        std::string new_var_name;

        std::string dst;
        virtual ~IRcode() = default;
        void generate();
        std::string dump();

    private:
        virtual void _generate(){};
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

    class IRJump : public IRcode
    {
    public:
        std::string Label;
        IRJump(std::string Label) : Label(Label) {}

    private:
        virtual void _generate() override;
    };

    class IRBranch : public IRcode
    {
    public:
        std::string Cond;
        std::string IfLabel;
        std::string ElseLabel;
        IRBranch(std::string Cond, std::string IfLabel, std::string ElseLabel) : Cond(Cond), IfLabel(IfLabel), ElseLabel(ElseLabel) {}

    private:
        virtual void _generate() override;
    };

    class IRGetPtr : public IRcode
    {
    public:
        int IRGetElementPtrType;
        // 0是一个针对全局数组的操作
        // 是一个针对局部数组的操作
        // 1并不是第一层了
        std::vector<int> this_lvl_shape;
        int Lvl;
        // 层数，shape几层它几层
        // 最少1层
        std::string Ptr;
        std::string Var;
        std::string Index;
        MC::IR::VarType Get_Type;
        IRGetPtr(std::string Ptr, std::string Var, std::string Index = "") : Ptr(Ptr), Var(Var), Index(Index) {}

    private:
        virtual void _generate() override;
    };

    class IRGetElementPtr : public IRcode
    {
    public:
        int IRGetElementPtrType;
        // 0是一个针对全局数组的操作
        // 是一个针对局部数组的操作
        // 1并不是第一层了
        std::vector<int> this_lvl_shape;
        int Lvl;
        // 层数，shape几层它几层
        // 最少1层
        std::string Ptr;
        std::string Arr;
        std::string Ind;
        MC::IR::VarType Get_Type;
        IRGetElementPtr(std::string Ptr, std::string Arr, std::string Ind, int IRGetElementPtrType, int Lvl, std::vector<int> this_lvl_shape)
            : Ptr(Ptr), Arr(Arr), Ind(Ind), IRGetElementPtrType(IRGetElementPtrType), Lvl(Lvl), this_lvl_shape(this_lvl_shape) {}

    private:
        virtual void _generate() override;
    };

    class IRStore : public IRcode
    {
    public:
        // store %x, %y
        // Mem[y]=x
        std::string Value;
        std::string AddressOfTarget;
        IRStore(std::string Value, std::string AddressOfTarget) : Value(Value), AddressOfTarget(AddressOfTarget) {}

    private:
        virtual void _generate() override;
    };

    class IRLoad : public IRcode
    {
    public:
        // %x = load %y
        // x=Mem[y]
        std::string Ptr;
        std::string Var;
        IRLoad(std::string Ptr, std::string Var) : Ptr(Ptr), Var(Var) {}

    private:
        virtual void _generate() override;
    };

    class IRGlobalVar : public IRcode
    {
    public:
        std::string Var;
        VarType AllocType;
        std::string InitList;
        IRGlobalVar(std::string Var, VarType AllocType, std::string InitList) : Var(Var), AllocType(AllocType), InitList(InitList) {}

    private:
        virtual void _generate() override;
    };

    class IRArrayDef : public IRcode
    {
    public:
        std::string Var;
        VarType AllocType;
        std::vector<int> Shape;
        IRArrayDef(std::string Var, VarType AllocType, std::vector<int> Shape) : Var(Var), AllocType(AllocType), Shape(Shape) {}

    private:
        virtual void _generate() override;
    };

    class IRGlobalArray : public IRcode
    {
    public:
        std::string Var;
        VarType AllocType;
        std::vector<int> Shape;
        std::string InitList;
        // Only for ir tree
        std::vector<int> initbuffer;
        IRGlobalArray(std::string Var, VarType AllocType, std::vector<int> Shape, std::string InitList, std::vector<int> initbuffer)
            : Var(Var), AllocType(AllocType), Shape(Shape), InitList(InitList), initbuffer(initbuffer) {}

    private:
        virtual void _generate() override;
    };

    class IRVoid : public IRcode
    {
    public:
        IRVoid() {}

    private:
        virtual void _generate() override;
    };

    using IRList = std::vector<std::unique_ptr<IRcode>>;

    class IRListWrapper
    {
    public:
        std::unique_ptr<IRList> irList;
        bool print_ir_line_number{true};
        IRListWrapper() : irList(std::make_unique<IRList>()), print_ir_line_number(MC::config::print_ir_line_number) {}
        void Generate();
        void Dump();
        void Dump(std::string filename);
        void Dump(std::ostream &os);
        std::string toString();
    };

}
