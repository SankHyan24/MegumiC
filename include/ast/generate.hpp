#pragma once
#include <ast/ast.hpp>
#include <memory>
extern int yyparse(std::unique_ptr<MC::AST::node::BaseAST> &ast);

namespace MC::AST::node
{
    std::unique_ptr<BaseAST> generate(FILE *input = stdin);
}