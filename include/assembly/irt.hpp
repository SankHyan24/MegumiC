#pragma once
#include <memory>
#include <sstream>
#include <vector>

#include <ir/ir.hpp>
#include <assembly/context.hpp>

namespace MC::ASM
{
    enum class IRArgType
    {
        I32,
        I32Star
    };

    class IRArgPair
    {
    public:
        IRArgType type;
        std::string varName;
    };

    // IR tree is the irtree constructed from the ircode,
    // the generate method is used to generate the
    class IRTree
    {
    public:
        void generate(Context &ctx, std::ostream &out);
        virtual ~IRTree() = default;

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out);
    };

    class IRTInst : public IRTree
    {
    public:
        MC::IR::IROp tag;
        std::string opname1, opname2, opname3;
        int imm;
        MC::IR::BinOp opcode;

        // a new var?
        bool is_new_var{false};
        bool is_alloc{false};

        // if use opname, is 2op or 3op
        bool is_2_op{false};
        bool is_imm{false};
        bool is_branch{false};
        bool is_jump{false};
        bool is_getptr{false};
        bool is_getelementptr{false};
        bool is_load{false};
        bool is_store{false};
        bool is_ret{false};
        bool is_global{false};

        // function call
        bool is_call{false};
        std::vector<std::string> params_name;

        // array and it's init
        bool is_array;
        std::vector<int> array_shape;
        std::vector<int> array_init_buffer;
        int count_need_stack_byte(Context &ctx);
        std::string getNewVarName() { return opname1; }

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTBasicBlock : public IRTree
    {
    public:
        std::string label;
        std::vector<std::unique_ptr<IRTInst>> instLists;
        // for generate
        int needStackByte{0};
        int count_need_stack_byte(Context &ctx);

    private:
        void _count_need_stack_byte(Context &ctx);
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTFunction : public IRTree
    {
    public:
        IRArgType retType;
        std::string functionName;
        // std::vector<IRArgPair> argList;
        std::vector<std::unique_ptr<IRTBasicBlock>> bbList;

        int needStackByte{0};

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTRoot : public IRTree
    {
    public:
        std::vector<std::unique_ptr<IRTInst>> instList;
        std::vector<std::unique_ptr<IRTFunction>> funcList;

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

}