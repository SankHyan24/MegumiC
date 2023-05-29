#include <ast/generate.hpp>
#include <cassert>
#include <fstream>

extern int yylex_destroy();
extern void yyset_lineno(int _line_number);
// extern int yycolumn;
void yyset_in(FILE *_in_str);

namespace MC::AST::node
{
    std::unique_ptr<BaseAST> generate(std::string input)
    {
        std::unique_ptr<BaseAST> ast;
        yyset_in(fmemopen((void *)input.c_str(), input.size(), "r"));
        yyset_lineno(1);
        auto res = yyparse(ast);
        assert(!res);
        yylex_destroy();
        return std::move(ast);
    }
}