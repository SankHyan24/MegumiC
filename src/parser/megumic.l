%option noyywrap

%{

#include <cstdlib>
#include <string>

// %option noinput
// %option nounput
// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件


#include "megumic.tab.hpp"

using namespace std;

void yyerror(const char *s);
int yycolumn = 1;


#define YY_USER_ACTION yylloc.first_line = yylineno;            \
                       yylloc.first_column = yycolumn;          \
                       for (auto i : std::string(yytext_ptr)) { \
                           if (i == '\n') {                     \
                               yylineno++;                      \
                               yycolumn = 1;                    \
                           } else {                             \
                               yycolumn++;                      \
                           }                                    \
                       }                                        \
                       yylloc.last_line = yylineno;             \
                       yylloc.last_column = yycolumn - 1;
%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

/* 单个字符 */
Char          '[^\\']'

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }
"/*"                        		{
                                        int c;
                                        while((c = yyinput()) != 0) {
                                            if(c == '\n') {
                                                yylineno++;
                                                yycolumn = 1;
                                            } else if(c == '*') {
                                                if((c = yyinput()) == '/')
                                                    break;
                                                else
                                                    unput(c);
                                                }
                                        }
                                    }

"int"           { return INT; }/*Key words */
"ptr"           { return PTR; }/*Key words */
"const"         { return CONST; }/*Key words */
"return"        { return RETURN; }
"static"      { return FUNCTION; }
"while"         { return WHILE; }
"if"            { return IF; }
"else"          { return ELSE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE;}/*单个字符：*/
{Char}         { yylval.int_val= yytext[1]; return CHAR_CONST; }


"<="            { return LE_OP; }
">="            { return GE_OP; }
"=="            { return EQ_OP; }
"!="            { return NE_OP; }
"&&"            { return AND_OP; }
"||"            { return OR_OP; }

";"     { return SEMICOLON; }
","     { return COMMA; }

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

.               { return yytext[0]; }

%%
