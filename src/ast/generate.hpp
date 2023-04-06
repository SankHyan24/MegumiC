#pragma once
#include <ast/ast.hpp>
#include <memory>
extern int yyparse(std::unique_ptr<MC::ast::BaseAST> &ast);

namespace MC::ast
{
    std::unique_ptr<BaseAST> generate(FILE *input = stdin);
}