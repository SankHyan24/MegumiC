#pragma once
#include <ast/ast.hpp>
#include <ir/ir.hpp>
#include <memory>

namespace MC::IR
{
    /***
     * @brief Generate IR from AST
     */
    std::unique_ptr<IRListWrapper> generate(std::unique_ptr<MC::AST::node::BaseAST> &ast);

}