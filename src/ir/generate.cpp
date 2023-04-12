#include <ir/generate.hpp>
#include <ir/context.hpp>
#include <cassert>

namespace MC::IR
{
    std::unique_ptr<IRList> generate(std::unique_ptr<MC::ast::node::BaseAST> &ast)
    {
        Context ctx;
        IRList ir;
        ast->generate_ir(ctx, ir);
        return std::make_unique<IRList>(std::move(ir));
    }
}