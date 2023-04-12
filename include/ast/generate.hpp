#pragma once
#include <ast/ast.hpp>
#include <memory>
extern int yyparse(std::unique_ptr<MC::ast::node::BaseAST> &ast);

namespace MC::ast::node
{
    std::unique_ptr<BaseAST> generate(FILE *input = stdin);
}