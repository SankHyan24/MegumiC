#include <ast/ast.hpp>
#include <stack>
using namespace MC::ast::node;

namespace
{
    std::stack<BaseAST *> nodes;
    int rec_depth = 0;
}
int BaseAST::rec_depth = 0;
void BaseAST::Dump()
{
    _printTabs();
    rec_depth++;
    this->_dump();
    rec_depth--;
}

void BaseAST::generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
{
    nodes.push(this);
    try
    {
        // this->_generate_ir(ctx, ir);
        nodes.pop();
    }
    catch (...)
    {
        nodes.pop();
        throw;
    }
}

void Expression::Dump()
{
    // _printTabs();
    this->_dump();
}

void Statement::Dump()
{
    _printTabs();
    rec_depth++;
    this->_dump();
    rec_depth--;
}
