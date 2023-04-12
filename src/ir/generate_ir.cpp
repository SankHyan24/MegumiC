#include <cassert>
#include <exception>
#include <unordered_set>

#include <ast/ast.hpp>
#include <config.hpp>
#include <ir/ir.hpp>

using namespace MC::IR;

namespace MC::ast::node
{
    void BaseAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        throw std::runtime_error("Not implemented");
    }

    void CompUnitAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        func_def->generate_ir(ctx, ir);
    }
}