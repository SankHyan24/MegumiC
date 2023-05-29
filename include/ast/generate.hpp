#pragma once
#include <ast/ast.hpp>
#include <memory>
#include <fstream>
extern int yyparse(std::unique_ptr<MC::AST::node::BaseAST> &ast);

namespace MC::AST::node
{
    // from istream
    std::unique_ptr<BaseAST> generate(std::string input);
}