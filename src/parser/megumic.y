%code requires {
	#include <memory>
	#include <string>
	#include "ast/ast.hpp"

}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast/ast.hpp"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<MC::ast::node::BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<MC::ast::node::BaseAST> &ast }
// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况


%union {
	int token;
	std::string *str_val;
	int int_val;
	MC::ast::node::BaseAST *ast_val;
	MC::ast::node::Expression *expression_val;
	MC::ast::node::Statement *statement_val;
	MC::IR::BinOp binop;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN 
%token <str_val> IDENT AND_OP OR_OP 
%token <int_val> INT_CONST 
%token <binop> LE_OP GE_OP EQ_OP NE_OP
/* %left LE_OP GE_OP EQ_OP NE_OP */
%type <binop>  MulOp RelOp AddOp EqOp
%type <int_val> UnaryOp  
%type <ast_val> FuncDef FuncType Block 
%type <expression_val> Exp UnaryExp PrimaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp Number
%type <statement_val> Stmt
%%


CompUnit
	: FuncDef { ast = move(make_unique<MC::ast::node::CompUnitAST>($1)); };


FuncDef
	: FuncType IDENT '(' ')' Block { $$ = new MC::ast::node::FuncDefAST($1,$2,$5);};

FuncType
	: INT { $$ = new MC::ast::node::FuncTypeAST(new string("int")); } ;

Block
	: '{' Stmt '}' { $$ = new MC::ast::node::BlockAST($2); } ;

Stmt
	: RETURN Exp ';'{ $$ = new MC::ast::node::ReturnStatement($2); } ;

Exp 
	: LOrExp ;

PrimaryExp
	: '(' Exp ')'{ $$ = $2;} 
	| Number;

Number
	: INT_CONST {$$ = new MC::ast::node::NumberAST($1);};

UnaryExp
	: PrimaryExp 
	| UnaryOp UnaryExp { $$= new MC::ast::node::UnaryExpression($1,$2);};

UnaryOp
	: '+' {$$ = '+';}
	| '-' {$$ = '-';}
	| '!' {$$ = '!';};

MulOp
	: '*' {$$ = MC::IR::BinOp::MUL;}
	| '/' {$$ = MC::IR::BinOp::DIV;}
	| '%' {$$ = MC::IR::BinOp::MOD;};

AddOp
	: '+' {$$ = MC::IR::BinOp::ADD;}
	| '-' {$$ = MC::IR::BinOp::SUB;};

RelOp
	: '<' {$$ = MC::IR::BinOp::LT;}
	| '>' {$$ = MC::IR::BinOp::GT;}
	| LE_OP {$$ =  MC::IR::BinOp::LE;}
	| GE_OP {$$ =  MC::IR::BinOp::GE;};

EqOp
	: EQ_OP {$$ = MC::IR::BinOp::EQ;}
	| NE_OP {$$ = MC::IR::BinOp::NE;};

MulExp
	: UnaryExp 
	| MulExp MulOp UnaryExp {$$ = new MC::ast::node::BinaryExpression($1,$2,$3);};

AddExp
	: MulExp 
	| AddExp AddOp MulExp {$$ = new MC::ast::node::BinaryExpression($1,$2,$3);};

RelExp
	: AddExp 
	| RelExp RelOp AddExp {$$ = new MC::ast::node::BinaryExpression($1,$2,$3);};

EqExp
	: RelExp
	| EqExp EqOp RelExp { $$ = new MC::ast::node::BinaryExpression($1,$2,$3);};

LAndExp
	: EqExp
	| LAndExp AND_OP EqExp { $$ = new MC::ast::node::BinaryExpression($1,MC::IR::BinOp::AND,$3);};

LOrExp
	: LAndExp 
	| LOrExp OR_OP LAndExp { $$ = new MC::ast::node::BinaryExpression($1,MC::IR::BinOp::OR,$3);};

%%

void yyerror(unique_ptr<MC::ast::node::BaseAST> &ast, const char *s) {
	cerr << "error: " << s << endl;
}
