#include <ir/generate.hpp>
#include <ir/context.hpp>
#include <cassert>

namespace MC::IR
{
    std::unique_ptr<IRListWrapper> generate(std::unique_ptr<MC::AST::node::BaseAST> &ast)
    {
        Context ctx; // the context
        IRList ir;
        ast->generate_ir(ctx, ir);
        IRListWrapper irListWrapper;
        irListWrapper.irList = std::make_unique<IRList>(std::move(ir));
        irListWrapper.Generate();
        return std::make_unique<IRListWrapper>(std::move(irListWrapper));
    }

}