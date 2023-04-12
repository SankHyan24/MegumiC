#include <ast/generate.hpp>
#include <cassert>

extern int yylex_destroy();
extern void yyset_lineno(int _line_number);
// extern int yycolumn;
void yyset_in(FILE *_in_str);

namespace MC::ast::node
{
    std::unique_ptr<BaseAST> generate(FILE *input)
    {
        std::unique_ptr<BaseAST> ast;
        yyset_in(input);
        yyset_lineno(1);
        // yycolumn = 1;
        auto res = yyparse(ast);
        assert(!res);
        yylex_destroy();
        // return the ast built、
        return std::move(ast);
    }
}