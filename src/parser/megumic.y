%code requires {
	#include <memory>
	#include <string>
	#include "ast/ast.hpp"
}

%{
#include "megumic.tab.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <ast/ast.hpp>
#include <config.hpp>
using namespace std;
extern int yydebug;

extern int yylex();
extern int yyget_lineno();
extern int yylex_destroy();

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<MC::AST::node::BaseAST> &ast, const char *s){
	    std::cerr << MC::config::config->getInputFile() << ':'<< yylloc.first_line << ':'
              << yylloc.first_column << ": error: " << s << std::endl;
    yylex_destroy();
    if (!yydebug) std::exit(1);
}
#define YYERROR_VERBOSE true
#define YYERROR_VERBOSE true
#ifdef YYDEBUG
#undef YYDEBUG
#endif
#define YYDEBUG 1
#define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do {                                                              \
        if (N) {                                                      \
            (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;    \
            (Current).first_column = YYRHSLOC (Rhs, 1).first_column;  \
            (Current).last_line    = YYRHSLOC (Rhs, N).last_line;     \
            (Current).last_column  = YYRHSLOC (Rhs, N).last_column;   \
        } else {                                                      \
            (Current).first_line   = (Current).last_line   =          \
              YYRHSLOC (Rhs, 0).last_line;                            \
            (Current).first_column = (Current).last_column =          \
              YYRHSLOC (Rhs, 0).last_column;                          \
        }                                                             \
        yylloc = Current;                                             \
    } while (0)
#define yytnamerr(_yyres, _yystr)                                         \
        ([](char* yyres, const char* yystr) {                             \
            if (*yystr == '"') {                                          \
                if (yyres) return yystpcpy(yyres, yystr + 1) - yyres - 1; \
                else return yystrlen(yystr) - 2;                          \
            } else {                                                      \
                if (yyres) return yystpcpy(yyres, yystr) - yyres;         \
                else return yystrlen(yystr);                              \
            }                                                             \
        })(_yyres, _yystr)
%}


// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<MC::AST::node::BaseAST> &ast }
// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%locations

%union {
	int token;
	int int_val;
	std::string *str_val;
	MC::AST::node::BaseAST *ast_val;
	MC::AST::node::Identifier* ident;
	MC::AST::node::Expression *expression_val;
	MC::AST::node::DeclareStatement *declare_stmt;
	MC::AST::node::FunctionDefine *function_def;
	MC::AST::node::Declare *declare;

	MC::AST::node::ArrayDeclareInitValue *array_declare_init_value;
	MC::AST::node::ArrayDeclare *array_delcare;
	MC::AST::node::ArrayIdentifier *array_identifier;

	MC::AST::node::FuncTypeAST *func_type;
	MC::AST::node::FunctionCallArgList *function_call_arg_list;
	MC::AST::node::FunctionDefineArgList *function_define_arg_list;
	MC::AST::node::FunctionDefineArg *function_define_arg;

	MC::AST::node::BlockAST *block_val;
	MC::AST::node::Statement *statement;
	MC::AST::node::Assignment *Assignment_stmt;
	MC::AST::node::IfElseStatement* ifelse_stmt;
	MC::AST::node::ConditionExpression* condition_expression;

	MC::AST::node::CompUnitAST* root;
	MC::IR::BinOp binop;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token <token> INT PTR RETURN CONST SEMICOLON COMMA FUNCTION WHILE IF ELSE BREAK CONTINUE
%token <str_val>  AND_OP OR_OP IDENT
%token <int_val> INT_CONST  CHAR_CONST
%token <binop> LE_OP GE_OP EQ_OP NE_OP
/* %left LE_OP GE_OP EQ_OP NE_OP */
/* %type <str_val>  */

%type <ident> ident LVal
%type <binop>  MulOp RelOp AddOp EqOp
%type <expression_val> Exp UnaryExp PrimaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp Number FunctionCall
%type <declare_stmt> Decl VarDecl ConstDecl  ConstDeclStmt VarDeclStmt  
%type <declare> VarDef ConstDef VarDefOne VarDefArray
// array
%type <array_identifier> DefArrayName ArrayItem

%type <function_def> FuncDef
%type <function_define_arg_list> FuncParams
%type <function_define_arg> FuncParam FuncParamArray FuncParamOne
%type <function_call_arg_list> FuncRParams
// arrayinit
%type <array_declare_init_value> InitValArray InitValArrayItems
// arglist
%type <block_val> Block BlockItems 
%type <statement> Stmt BlockItem BreakStmt ReturnStmt WhileStmt  IfStmt AssignStmt Assignment ContinueStmt 
/* %type <statement> ForStmt ContinueStmt */
%type <int_val> UnaryOp  
%type <func_type> FuncType BType 

%type <root> CompUnit
//
%start CompUnit
%%


CompUnit
	: CompUnit Decl { 
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::AST::node::DeclareStatement>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.back().reset($2);
	}	| CompUnit FuncDef { 
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::AST::node::FunctionDefine>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.back().reset($<function_def>2);
	}	| Decl { 
		ast = std::move(make_unique<MC::AST::node::CompUnitAST>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::AST::node::DeclareStatement>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.back().reset($1);
	}	| FuncDef { 
		ast = std::move(make_unique<MC::AST::node::CompUnitAST>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::AST::node::FunctionDefine>());
		((MC::AST::node::CompUnitAST*)(ast.get()))->list.back().reset($<function_def>1);
	};

// 函数定义
FuncDef
	: FUNCTION FuncType ident '(' ')' Block { $$ = new MC::AST::node::FunctionDefine($2,$3,(new MC::AST::node::FunctionDefineArgList()),$6);}
	| FUNCTION FuncType ident '(' FuncParams ')' Block { $$ = new MC::AST::node::FunctionDefine($2,$3,$5,$7);};
	/* : FuncType ident '(' ')' Block { $$ = new MC::AST::node::FunctionDefine($1,$2,(new MC::AST::node::FunctionDefineArgList()),$5);}
	| FuncType ident '(' FuncParams ')' Block { $$ = new MC::AST::node::FunctionDefine($1,$2,$4,$6);}; */

FuncParams
	: FuncParams COMMA FuncParam { $$->list.push_back(std::unique_ptr<MC::AST::node::FunctionDefineArg>());
		$$->list.back().reset($3);
	}
	| FuncParam { $$ = new MC::AST::node::FunctionDefineArgList(); 
	$$->list.push_back(std::unique_ptr<MC::AST::node::FunctionDefineArg>());
	$$->list.back().reset($1);
	};

FuncParam: FuncParamOne
          | FuncParamArray
          ;

FuncParamOne
	: BType ident { $$ = new MC::AST::node::FunctionDefineArg($1, $2); };

FuncParamArray
	: FuncParamArray '[' Exp ']' {
		$$ = $1;
		((MC::AST::node::ArrayIdentifier*)($$->name.get()))->index_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		((MC::AST::node::ArrayIdentifier*)($$->name.get()))->index_list.back().reset($3);
	}
	| FuncParamOne {
		$$ = new MC::AST::node::FunctionDefineArg($1->type.get(), new MC::AST::node::ArrayIdentifier($1->name->name));
	}

FuncType
	: INT { $$ = new MC::AST::node::FuncTypeAST(MC::IR::VarType::Val); } ;
	| PTR { $$ = new MC::AST::node::FuncTypeAST(MC::IR::VarType::Ptr); } ;



FunctionCall
	: ident '(' ')' { $$ = new MC::AST::node::FunctionCall($1, new MC::AST::node::FunctionCallArgList()); }
	| ident '(' FuncRParams ')' { $$ = new MC::AST::node::FunctionCall($1, $3); };


FuncRParams
	: FuncRParams COMMA AddExp { 
		$$->arg_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->arg_list.back().reset($3);
	}
	| AddExp { 
		$$ = new MC::AST::node::FunctionCallArgList(); 
		$$->arg_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->arg_list.back().reset($1);
	};

Block
	: '{' '}'{ $$ = new MC::AST::node::BlockAST();}
	| '{' BlockItems '}' { $$ = $2; };
	 

BlockItems
	: BlockItem { $$ = new MC::AST::node::BlockAST(); 
		$$->stmt.push_back(std::unique_ptr<MC::AST::node::Statement>());
		$$->stmt.back().reset($1);
	}
	| BlockItems BlockItem { $$ = $1;
		$$->stmt.push_back(std::unique_ptr<MC::AST::node::Statement>());
		$$->stmt.back().reset($2);
	};

BlockItem
	: Decl | Stmt;

// 声明语句
Decl
	: ConstDeclStmt
	| VarDeclStmt

// 常数声明语句 const int a=1,b=2;
ConstDeclStmt
	: ConstDecl SEMICOLON { $$ = $1; } ;

ConstDecl
	: CONST BType ConstDef { $$ = new MC::AST::node::DeclareStatement($2); 
		$$->list.push_back(std::unique_ptr<MC::AST::node::Declare>());
		$$->list.back().reset($3);
	} 
	| ConstDecl COMMA ConstDef { $$->list.push_back(std::unique_ptr<MC::AST::node::Declare>());
		$$->list.back().reset($3);
	};

ConstDef 
	: ident '=' Exp { $$ = new MC::AST::node::VarDeclareWithInit($1, $3, true);};


VarDeclStmt
	: VarDecl SEMICOLON { $$ = $1; } ;

VarDecl
	: BType VarDef {$$ = new MC::AST::node::DeclareStatement($1); 
		$$->list.push_back(std::unique_ptr<MC::AST::node::Declare>());
		$$->list.back().reset($2);
	} 
	| VarDecl COMMA VarDef { 
		$$->list.push_back(std::unique_ptr<MC::AST::node::Declare>());
		$$->list.back().reset($3);
	};

VarDef
	: VarDefOne
	| VarDefArray
	;

VarDefOne
	: ident '=' Exp {$$ = new MC::AST::node::VarDeclareWithInit($1, $3, false); }
	| ident { $$ =  new MC::AST::node::VarDeclare($1);};

VarDefArray
	: DefArrayName '=' InitValArray { $$ = new MC::AST::node::ArrayDeclareWithInit($1,$3);}
	| DefArrayName { $$ = new MC::AST::node::ArrayDeclare($1); }

InitValArray
	: '{' InitValArrayItems '}' { $$ = $2; }
	| '{' '}' { $$ = new MC::AST::node::ArrayDeclareInitValue(false, nullptr);};// ?

InitValArrayItems
	: InitValArrayItems COMMA InitValArray { $$ = $1;
		$$->value_list.push_back(std::unique_ptr<MC::AST::node::ArrayDeclareInitValue>());
		$$->value_list.back().reset($3);
	}
	| InitValArrayItems COMMA AddExp { $$ = $1;
		$$->value_list.push_back(std::unique_ptr<MC::AST::node::ArrayDeclareInitValue>());
		auto tmp = new MC::AST::node::ArrayDeclareInitValue(true, $3);
		$$->value_list.back().reset(tmp);//
	}
	| InitValArray { $$ = new MC::AST::node::ArrayDeclareInitValue(true, nullptr);
		$$->value_list.push_back(std::unique_ptr<MC::AST::node::ArrayDeclareInitValue>());
		$$->value_list.back().reset($1);
	}
	| AddExp { $$ = new MC::AST::node::ArrayDeclareInitValue(true, nullptr);
		$$->value_list.push_back(std::unique_ptr<MC::AST::node::ArrayDeclareInitValue>());
		auto tmp = new MC::AST::node::ArrayDeclareInitValue(true, $1);
		$$->value_list.back().reset(tmp);// 
	};

DefArrayName
	: DefArrayName '[' AddExp ']' { $$ = $1;
		$$->index_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->index_list.back().reset($3);
	}
	| ident '[' AddExp ']' { $$ = new MC::AST::node::ArrayIdentifier($1);
		$$->index_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->index_list.back().reset($3);
	};

BType
	: FuncType;

Stmt
	: Block
	| ReturnStmt
	| WhileStmt
	| BreakStmt
	| IfStmt
	| AssignStmt
	| ContinueStmt
	| Exp SEMICOLON { $$ = new  MC::AST::node::EvaluateStatement($1); }
	| SEMICOLON { $$ = new  MC::AST::node::VoidStatement(); }

ReturnStmt
	: RETURN Exp SEMICOLON { $$ = new MC::AST::node::ReturnStatement($2); } ;
	| RETURN SEMICOLON { $$ = new MC::AST::node::ReturnStatement(new MC::AST::node::NumberAST(0)); } ;

WhileStmt
	: WHILE '(' Exp ')' Stmt { $$ = new MC::AST::node::WhileStatement($3, $5); } ;

BreakStmt
	: BREAK SEMICOLON { $$ = new MC::AST::node::BreakStatement(); } ;
ContinueStmt
	: CONTINUE SEMICOLON { $$ = new MC::AST::node::ContinueStatement(); } ;
IfStmt
	: IF '(' Exp ')' Stmt { $$ = new MC::AST::node::IfElseStatement($3, $5,new MC::AST::node::VoidStatement()); }
	| IF '(' Exp ')' Stmt ELSE Stmt { $$ = new MC::AST::node::IfElseStatement($3, $5, $7); };
AssignStmt
	: Assignment SEMICOLON { $$ = $1; } ;
Assignment
	: LVal '=' Exp { $$ = new MC::AST::node::Assignment($1, $3); };

//
Exp 
	: LOrExp ;

PrimaryExp
	: '(' Exp ')'{ $$ = $2;} 
	| LVal
	| Number;

Number
	: INT_CONST {$$ = new MC::AST::node::NumberAST($1);};
	| CHAR_CONST {$$ = new MC::AST::node::NumberAST($1);};

UnaryExp
	: PrimaryExp 
	| FunctionCall
	| UnaryOp UnaryExp { $$= new MC::AST::node::UnaryExpression($1,$2);};

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
	| MulExp MulOp UnaryExp {$$ = new MC::AST::node::BinaryExpression($1,$2,$3);};

AddExp
	: MulExp 
	| AddExp AddOp MulExp {$$ = new MC::AST::node::BinaryExpression($1,$2,$3);};

RelExp
	: AddExp 
	| RelExp RelOp AddExp {$$ = new MC::AST::node::BinaryExpression($1,$2,$3);};

EqExp
	: RelExp
	| EqExp EqOp RelExp { $$ = new MC::AST::node::BinaryExpression($1,$2,$3);};

LAndExp
	: EqExp
	| LAndExp AND_OP EqExp { $$ = new MC::AST::node::BinaryExpression($1,MC::IR::BinOp::AND,$3);};

LOrExp
	: LAndExp 
	| LOrExp OR_OP LAndExp { $$ = new MC::AST::node::BinaryExpression($1,MC::IR::BinOp::OR,$3);};

ident: IDENT { $$ = new MC::AST::node::Identifier(*$1); };
LVal: ident
	| ArrayItem;
ArrayItem
	: LVal '[' Exp ']'{ 
		$$ = new MC::AST::node::ArrayIdentifier($1);
		$$->index_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->index_list.back().reset($3);
	}
	| ArrayItem '[' Exp ']'{
		$$ = $1;
		$$->index_list.push_back(std::unique_ptr<MC::AST::node::Expression>());
		$$->index_list.back().reset($3);
	};
%%

