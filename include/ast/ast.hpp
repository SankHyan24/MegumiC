#pragma once
#include <iostream>
#include <memory>

#include <ir/ir.hpp>
#include <ir/context.hpp>
// 所有 AST 的基类
namespace MC::ast::node
{

	class BaseAST
	{
	public:
		static int rec_depth;
		virtual ~BaseAST() = default;

		virtual void Dump(); // Dump the AST
		void generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir);

	protected:
		static void _printTabs(int j = 0) // This function only used in dump
		{
			std::string tabs = "";
			int len = std::max(0, rec_depth + j);
			for (int i = 0; i < len; i++)
				tabs += "\t";
			std::cout << tabs;
		}

	private:
		virtual void _dump() const = 0;
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir);
	};

	class Expression : public BaseAST
	{
	public:
		void Dump();

	private:
		virtual void _dump() const = 0;
	};

	class Statement : public Expression
	{
	public:
		void Dump();

	private:
		virtual void _dump() const = 0;
	};

	class Identifier : public Expression
	{
	public:
		std::string name;
		Identifier(std::string name) : name(name) {}

	private:
		virtual void _dump() const override
		{
			std::cout << "Identifier {" << name << "}" << std::endl;
		}
	};

	class Declare : public BaseAST
	{
	};

	class CompUnitAST : public BaseAST
	{
	public:
		std::vector<std::unique_ptr<BaseAST>> list;
		CompUnitAST() {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "CompUnitAST {" << std::endl;
			for (auto &i : list)
				i->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
	};

	class FuncTypeAST : public BaseAST
	{
	public:
		std::unique_ptr<std::string> type_name;
		FuncTypeAST(std::string *arg) : type_name(std::move(arg)) {}

	private:
		void _dump() const override
		{
			std::cout << "FuncTypeAST {" << *type_name << "}" << std::endl;
		}
	};

	class BlockAST : public BaseAST
	{
	public:
		std::vector<std::unique_ptr<Statement>> stmt;
		BlockAST() {}
		// BlockAST(const BlockAST &c)
		// {
		// 	for (auto &i : c.stmt)
		// 	{
		// 		this->stmt.push_back(i);
		// 	}
		// }

	private:
		void _dump() const override
		{
			std::cout << "BlockAST {" << std::endl;
			for (auto &i : stmt)
				i->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
	};

	class NumberAST : public Expression
	{
	public:
		int number;
		NumberAST(int number) : number(number) {}
		NumberAST(const std::string &value) : number(std::stoi(value, 0, 0)) {}

	private:
		void _dump() const override
		{
			std::cout << number;
		}
	};

	class BinaryExpression : public Expression
	{
	public:
		MC::IR::BinOp op;
		std::unique_ptr<Expression> lhs, rhs;
		BinaryExpression(Expression *lhs, MC::IR::BinOp op, Expression *rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

	private:
		void _dump() const override
		{
			std::cout << " binE( ";
			this->lhs->Dump();
			std::cout << " " << this->op << " ";
			this->rhs->Dump();
			std::cout << " ) ";
		}
	};

	class UnaryExpression : public Expression
	{
	public:
		char op;
		std::unique_ptr<Expression> rhs;
		UnaryExpression(int op, Expression *rhs) : op(op), rhs(std::move(rhs)) {}

	private:
		void _dump() const override
		{
			std::cout << "UnaE" << this->op << "{ ";
			this->rhs->Dump();
			std::cout << " }";
		}
	};

	class ConditionExpression : public Expression
	{
	public:
		std::unique_ptr<Expression> ExpressionValue;
		ConditionExpression(Expression *ExpressionValue) : ExpressionValue(std::move(ExpressionValue)) {}

	private:
		void _dump() const override
		{
			std::cout << "ConditionExpression { ";
			this->ExpressionValue->Dump();
			std::cout << " }";
		}
	};

	// Function Call Arglist
	class FunctionCallArgList : public Expression
	{
	public:
		std::vector<std::unique_ptr<Expression>> arg_list;

	private:
		void _dump() const override
		{
			std::cout << "FunctionCallArglist { ";
			for (auto &i : arg_list)
				i->Dump();
			std::cout << " }";
		}
	};
	// Function Call
	class FunctionCall : public Expression
	{
	public:
		std::unique_ptr<Identifier> functionName;
		std::unique_ptr<FunctionCallArgList> arg_list;
		FunctionCall(Identifier *functionName, FunctionCallArgList *arg_list) : functionName(std::move(functionName)), arg_list(std::move(arg_list)) {}

	private:
		void _dump() const override
		{
			std::cout << "FunctionCall { ";
			functionName->Dump();
			arg_list->Dump();
			std::cout << " }";
		}
	};

	// Assignment
	class Assignment : public Statement
	{
	public:
		std::unique_ptr<Identifier> ident;
		std::unique_ptr<Expression> exp;
		Assignment(Identifier *ident, Expression *exp) : ident(std::move(ident)), exp(std::move(exp)) {}

	private:
		void _dump() const override
		{
			std::cout << "Assignment { ";
			ident->Dump();
			exp->Dump();
			std::cout << " }";
		}
	};
	// AfterInc

	// IfElseStatement
	class IfElseStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> if_statement;
		std::unique_ptr<Statement> else_statement;
		IfElseStatement(Expression *condition, Statement *if_statement, Statement *else_statement) : condition(std::move(condition)), if_statement(std::move(if_statement)), else_statement(std::move(else_statement)) {}

	private:
		void _dump() const override
		{
			std::cout << "IfElseStatement { if(";
			condition->Dump();
			std::cout << ") ";
			if_statement->Dump();
			std::cout << " else ";
			else_statement->Dump();
			std::cout << " }";
		}
	};
	// WhileStatement
	class WhileStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> stmt;
		WhileStatement(Expression *condition, Statement *stmt) : condition(std::move(condition)), stmt(std::move(stmt)) {}

	private:
		void _dump() const override
		{
			std::cout << "WhileStatement { condition(";
			condition->Dump();
			std::cout << " ) ";
			stmt->Dump();
			std::cout << " }";
		}
	};
	// ForStatement
	class BreakStatement : public Statement
	{
	public:
		BreakStatement() {}

	private:
		void _dump() const override
		{
			std::cout << "BreakStatement { }";
		}
	};
	// ContinueStatement
	class EvaluateStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> value;
		EvaluateStatement(Expression *value) : value(std::move(value)) {}

	private:
		void _dump() const override
		{
			std::cout << "EvaluateStatement { ";
			value->Dump();
			std::cout << " }";
		}
	};

	class ReturnStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> exp;
		ReturnStatement(Expression *exp) : exp(std::move(exp)) {}
		ReturnStatement(int number) : exp(std::move(new NumberAST(number))) {}

	private:
		void _dump() const override
		{
			std::cout << "ReturnStatement { ";
			exp->Dump();
			std::cout << " }" << std::endl;
		}
	};

	class DeclareStatement : public Statement
	{
	public:
		int type;
		std::vector<std::unique_ptr<Declare>> list;
		DeclareStatement(int type) : type(type){};

	private:
		void _dump() const override
		{
			std::cout << "DeclareStatement { ";
			for (int i = 0; i < list.size(); i++)
			{
				list.at(i)->Dump();
				if (i != list.size() - 1)
					std::cout << ", ";
			}
			std::cout << " }" << std::endl;
		}
	};

	class VoidStatement : public Statement
	{
	public:
	private:
		void _dump() const override
		{
			std::cout << "VoidStatement { ";
			std::cout << " }" << std::endl;
		}
	};

	class VarDeclare : public Declare
	{
	public:
		std::unique_ptr<Identifier> name;
		VarDeclare(Identifier *name) : name(std::move(name)){};

	private:
		void _dump() const override
		{
			std::cout << "VarDeclare { ";
			name->Dump();
			std::cout << " }" << std::endl;
		}
	};

	class VarDeclareWithInit : public Declare
	{
	public:
		std::unique_ptr<Identifier> name;
		std::unique_ptr<Expression> init_value;
		bool is_const;
		VarDeclareWithInit(Identifier *name, Expression *init_value, bool is_const = false)
			: name(std::move(name)), init_value(std::move(init_value)), is_const(is_const){};

	private:
		void _dump() const override
		{
			std::cout << "VarDeclareWithInit { ";
			name->Dump();
			std::cout << "=";
			init_value->Dump();
			std::cout << " }" << std::endl;
		}
	};

	class ArrayIdentifier : public Identifier
	{
	public:
		std::vector<std::unique_ptr<Expression>> index_list;
		ArrayIdentifier(std::string name) : Identifier(name){};
		ArrayIdentifier(Identifier *ident) : Identifier(*ident){};

	private:
		void _dump() const override
		{
			std::cout << "ArrayIdentifier { ";
			std::cout << name;
			for (auto &i : index_list)
			{
				std::cout << "[";
				i->Dump();
				std::cout << "]";
			}
			std::cout << " }";
		}
	};

	class ArrayDeclareInitValue : public Expression
	{
	public:
		bool is_const;
		std::unique_ptr<Expression> value;
		std::vector<std::unique_ptr<ArrayDeclareInitValue>> value_list;
		ArrayDeclareInitValue(bool is_const, Expression *value) : is_const(is_const), value(std::move(value)){};
		ArrayDeclareInitValue(bool is_const) : is_const(is_const){};

	private:
		void _dump() const override
		{
			if (value == nullptr)
			{
				std::cout << "{";
				for (int i = 0; i < value_list.size(); i++)
				{
					value_list.at(i)->Dump();
					if (i != value_list.size() - 1)
						std::cout << ", ";
				}
				std::cout << "}";
			}
			else
			{
				value->Dump();
			}
		}
	};

	class ArrayDeclareWithInit : public Declare
	{
	public:
		std::unique_ptr<ArrayIdentifier> name;
		std::unique_ptr<Expression> init_value;
		bool is_const;
		ArrayDeclareWithInit(ArrayIdentifier *name, Expression *init_value, bool is_const = false)
			: name(std::move(name)), init_value(std::move(init_value)), is_const(is_const){};

	private:
		void _dump() const override
		{
			std::cout << "ArrayDeclareWithInit { ";
			name->Dump();
			std::cout << "=";
			init_value->Dump();
			std::cout << " }" << std::endl;
		}
	};

	class ArrayDeclare : public Declare
	{
	public:
		std::unique_ptr<ArrayIdentifier> name;
		ArrayDeclare(ArrayIdentifier *name) : name(std::move(name)){};

	private:
		void _dump() const override
		{
		}
	};

	class FunctionDefineArg : public Expression
	{
	public:
		int type; // TODO
		std::unique_ptr<Identifier> name;
		FunctionDefineArg(int type, Identifier *name) : type(type), name(std::move(name)) {}

	private:
		void _dump() const override
		{
			std::cout << "FunctionDefineArg { ";
			name->Dump();
			std::cout << " }";
		}
	};

	class FunctionDefineArgList : public Expression
	{
	public:
		std::vector<std::unique_ptr<FunctionDefineArg>> list;

	private:
		void _dump() const override
		{
			std::cout << "FunctionDefineArgList { ";
			for (int i = 0; i < list.size(); i++)
			{
				list.at(i)->Dump();
				if (i != list.size() - 1)
					std::cout << ", ";
			}
			std::cout << " }";
		}
	};

	class FunctionDefine : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> func_type;
		std::unique_ptr<Identifier> name;
		std::unique_ptr<FunctionDefineArgList> arg_list;
		std::unique_ptr<BlockAST> block;
		FunctionDefine(BaseAST *return_type, Identifier *name, FunctionDefineArgList *arg_list, BlockAST *block) : func_type(return_type), name(std::move(name)), arg_list(std::move(arg_list)), block(std::move(block)) {}

	private:
		void _dump() const override
		{
			std::cout << "FunctionDefine { ";
			func_type->Dump();
			arg_list->Dump();
			name->Dump();
			block->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
	};
}