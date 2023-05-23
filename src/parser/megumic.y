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
	int int_val;
	std::string *str_val;
	MC::ast::node::BaseAST *ast_val;
	MC::ast::node::Identifier* ident;
	MC::ast::node::Expression *expression_val;
	MC::ast::node::DeclareStatement *declare_stmt;
	MC::ast::node::FunctionDefine *function_def;
	MC::ast::node::Declare *declare;

	MC::ast::node::ArrayDeclareInitValue *array_declare_init_value;
	MC::ast::node::ArrayDeclare *array_delcare;
	MC::ast::node::ArrayIdentifier *array_identifier;

	MC::ast::node::FuncTypeAST *func_type;
	MC::ast::node::FunctionCallArgList *function_call_arg_list;
	MC::ast::node::FunctionDefineArgList *function_define_arg_list;
	MC::ast::node::FunctionDefineArg *function_define_arg;

	MC::ast::node::BlockAST *block_val;
	MC::ast::node::Statement *statement;
	MC::ast::node::Assignment *Assignment_stmt;
	MC::ast::node::IfElseStatement* ifelse_stmt;
	MC::ast::node::ConditionExpression* condition_expression;

	MC::ast::node::CompUnitAST* root;
	MC::IR::BinOp binop;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token <token> INT PTR RETURN CONST SEMICOLON COMMA FUNCTION WHILE IF ELSE BREAK CONTINUE
%token <str_val>  AND_OP OR_OP IDENT
%token <int_val> INT_CONST 
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
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::ast::node::DeclareStatement>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.back().reset($2);
	}	| CompUnit FuncDef { 
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::ast::node::FunctionDefine>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.back().reset($<function_def>2);
	}	| Decl { 
		ast = move(make_unique<MC::ast::node::CompUnitAST>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::ast::node::DeclareStatement>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.back().reset($1);
	}	| FuncDef { 
		ast = move(make_unique<MC::ast::node::CompUnitAST>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.push_back(std::unique_ptr<MC::ast::node::FunctionDefine>());
		((MC::ast::node::CompUnitAST*)(ast.get()))->list.back().reset($<function_def>1);
	};

// 函数定义
FuncDef
	: FUNCTION FuncType ident '(' ')' Block { $$ = new MC::ast::node::FunctionDefine($2,$3,(new MC::ast::node::FunctionDefineArgList()),$6);}
	| FUNCTION FuncType ident '(' FuncParams ')' Block { $$ = new MC::ast::node::FunctionDefine($2,$3,$5,$7);};
	/* : FuncType ident '(' ')' Block { $$ = new MC::ast::node::FunctionDefine($1,$2,(new MC::ast::node::FunctionDefineArgList()),$5);}
	| FuncType ident '(' FuncParams ')' Block { $$ = new MC::ast::node::FunctionDefine($1,$2,$4,$6);}; */

FuncParams
	: FuncParams COMMA FuncParam { $$->list.push_back(std::unique_ptr<MC::ast::node::FunctionDefineArg>());
		$$->list.back().reset($3);
	}
	| FuncParam { $$ = new MC::ast::node::FunctionDefineArgList(); 
	$$->list.push_back(std::unique_ptr<MC::ast::node::FunctionDefineArg>());
	$$->list.back().reset($1);
	};

FuncParam: FuncParamOne
          | FuncParamArray
          ;

FuncParamOne
	: BType ident { $$ = new MC::ast::node::FunctionDefineArg($1, $2); };

FuncParamArray
	: FuncParamArray '[' Exp ']' {
		$$ = $1;
		((MC::ast::node::ArrayIdentifier*)($$->name.get()))->index_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		((MC::ast::node::ArrayIdentifier*)($$->name.get()))->index_list.back().reset($3);
	}
	| FuncParamOne {
		$$ = new MC::ast::node::FunctionDefineArg($1->type.get(), new MC::ast::node::ArrayIdentifier($1->name->name));
	}

FuncType
	: INT { $$ = new MC::ast::node::FuncTypeAST(MC::IR::VarType::Val); } ;
	| PTR { $$ = new MC::ast::node::FuncTypeAST(MC::IR::VarType::Ptr); } ;



FunctionCall
	: ident '(' ')' { $$ = new MC::ast::node::FunctionCall($1, new MC::ast::node::FunctionCallArgList()); }
	| ident '(' FuncRParams ')' { $$ = new MC::ast::node::FunctionCall($1, $3); };


FuncRParams
	: FuncRParams COMMA AddExp { 
		$$->arg_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		$$->arg_list.back().reset($3);
	}
	| AddExp { 
		$$ = new MC::ast::node::FunctionCallArgList(); 
		$$->arg_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		$$->arg_list.back().reset($1);
	};

Block
	: '{' '}'{ $$ = new MC::ast::node::BlockAST();}
	| '{' BlockItems '}' { $$ = $2; };
	 

BlockItems
	: BlockItem { $$ = new MC::ast::node::BlockAST(); 
		$$->stmt.push_back(std::unique_ptr<MC::ast::node::Statement>());
		$$->stmt.back().reset($1);
	}
	| BlockItems BlockItem { $$ = $1;
		$$->stmt.push_back(std::unique_ptr<MC::ast::node::Statement>());
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
	: CONST BType ConstDef { $$ = new MC::ast::node::DeclareStatement($2); 
		$$->list.push_back(std::unique_ptr<MC::ast::node::Declare>());
		$$->list.back().reset($3);
	} 
	| ConstDecl COMMA ConstDef { $$->list.push_back(std::unique_ptr<MC::ast::node::Declare>());
		$$->list.back().reset($3);
	};

ConstDef 
	: ident '=' Exp { $$ = new MC::ast::node::VarDeclareWithInit($1, $3, true);};


VarDeclStmt
	: VarDecl SEMICOLON { $$ = $1; } ;

VarDecl
	: BType VarDef {$$ = new MC::ast::node::DeclareStatement($1); 
		$$->list.push_back(std::unique_ptr<MC::ast::node::Declare>());
		$$->list.back().reset($2);
	} 
	| VarDecl COMMA VarDef { 
		$$->list.push_back(std::unique_ptr<MC::ast::node::Declare>());
		$$->list.back().reset($3);
	};

VarDef
	: VarDefOne
	| VarDefArray
	;

VarDefOne
	: ident '=' Exp {$$ = new MC::ast::node::VarDeclareWithInit($1, $3, true); }
	| ident { $$ =  new MC::ast::node::VarDeclare($1);};

VarDefArray
	: DefArrayName '=' InitValArray { $$ = new MC::ast::node::ArrayDeclareWithInit($1,$3);}
	| DefArrayName { $$ = new MC::ast::node::ArrayDeclare($1); }

InitValArray
	: '{' InitValArrayItems '}' { $$ = $2; }
	| '{' '}' { $$ = new MC::ast::node::ArrayDeclareInitValue(false, nullptr);};// ?

InitValArrayItems
	: InitValArrayItems COMMA InitValArray { $$ = $1;
		$$->value_list.push_back(std::unique_ptr<MC::ast::node::ArrayDeclareInitValue>());
		$$->value_list.back().reset($3);
	}
	| InitValArrayItems COMMA AddExp { $$ = $1;
		$$->value_list.push_back(std::unique_ptr<MC::ast::node::ArrayDeclareInitValue>());
		auto tmp = new MC::ast::node::ArrayDeclareInitValue(true, $3);
		$$->value_list.back().reset(tmp);//
	}
	| InitValArray { $$ = new MC::ast::node::ArrayDeclareInitValue(true, nullptr);
		$$->value_list.push_back(std::unique_ptr<MC::ast::node::ArrayDeclareInitValue>());
		$$->value_list.back().reset($1);
	}
	| AddExp { $$ = new MC::ast::node::ArrayDeclareInitValue(true, nullptr);
		$$->value_list.push_back(std::unique_ptr<MC::ast::node::ArrayDeclareInitValue>());
		auto tmp = new MC::ast::node::ArrayDeclareInitValue(true, $1);
		$$->value_list.back().reset(tmp);// 
	};

DefArrayName
	: DefArrayName '[' AddExp ']' { $$ = $1;
		$$->index_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		$$->index_list.back().reset($3);
	}
	| ident '[' AddExp ']' { $$ = new MC::ast::node::ArrayIdentifier($1);
		$$->index_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
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
	| Exp SEMICOLON { $$ = new  MC::ast::node::EvaluateStatement($1); }
	| SEMICOLON { $$ = new  MC::ast::node::VoidStatement(); }

ReturnStmt
	: RETURN Exp SEMICOLON { $$ = new MC::ast::node::ReturnStatement($2); } ;

WhileStmt
	: WHILE '(' Exp ')' Stmt { $$ = new MC::ast::node::WhileStatement($3, $5); } ;

BreakStmt
	: BREAK SEMICOLON { $$ = new MC::ast::node::BreakStatement(); } ;
ContinueStmt
	: CONTINUE SEMICOLON { $$ = new MC::ast::node::ContinueStatement(); } ;
IfStmt
	: IF '(' Exp ')' Stmt { $$ = new MC::ast::node::IfElseStatement($3, $5,new MC::ast::node::VoidStatement()); }
	| IF '(' Exp ')' Stmt ELSE Stmt { $$ = new MC::ast::node::IfElseStatement($3, $5, $7); };
AssignStmt
	: Assignment SEMICOLON { $$ = $1; } ;
Assignment
	: LVal '=' Exp { $$ = new MC::ast::node::Assignment($1, $3); };

//
Exp 
	: LOrExp ;

PrimaryExp
	: '(' Exp ')'{ $$ = $2;} 
	| LVal
	| Number;

Number
	: INT_CONST {$$ = new MC::ast::node::NumberAST($1);};

UnaryExp
	: PrimaryExp 
	| FunctionCall
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

ident: IDENT { $$ = new MC::ast::node::Identifier(*$1); };
LVal: ident
	| ArrayItem;
ArrayItem
	: LVal '[' Exp ']'{ 
		$$ = new MC::ast::node::ArrayIdentifier($1);
		$$->index_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		$$->index_list.back().reset($3);
	}
	| ArrayItem '[' Exp ']'{
		$$ = $1;
		$$->index_list.push_back(std::unique_ptr<MC::ast::node::Expression>());
		$$->index_list.back().reset($3);
	};
%%

void yyerror(unique_ptr<MC::ast::node::BaseAST> &ast, const char *s) {
	cerr << "error: " << s << endl;
}



