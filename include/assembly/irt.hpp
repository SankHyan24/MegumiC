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
        virtual void _generate(Context &ctx, std::ostream &out);
    };

    class IRTInst : public IRTree
    {
    public:
        MC::IR::IROp tag;
        std::string opname1, opname2, opname3;
        MC::IR::BinOp opcode;

        bool if_array;
        std::vector<int> array_shape;
        std::vector<int> array_init_buffer;

    private:
        virtual void _generate(Context &ctx, std::ostream &out) override;
    };

    class IRTBasicBlock : public IRTree
    {
    public:
        std::vector<std::unique_ptr<IRTInst>> instLists;

    private:
        // virtual void _generate(Context &ctx, std::ostream &out) override;
    };

    class IRTFunction : public IRTree
    {
    public:
        std::vector<IRArgType> retType;
        std::vector<IRArgPair> argList;
        std::vector<std::unique_ptr<IRTBasicBlock>> bbList;

    private:
        // virtual void _generate(Context &ctx, std::ostream &out) override;
    };

    class IRTRoot : public IRTree
    {
    public:
        std::vector<std::unique_ptr<IRTInst>> instList;
        std::vector<std::unique_ptr<IRTFunction>> funcList;

    private:
        // virtual void _generate(Context &ctx, std::ostream &out) override;
    };

}