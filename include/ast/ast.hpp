#pragma once
#include <iostream>
#include <memory>
#include <cassert>

#include <ir/ir.hpp>
#include <ir/context.hpp>
// 所有 AST 的基类
namespace MC::ast::node
{

	class BaseAST
	{
	public:
		int id;
		static int rec_depth;
		virtual ~BaseAST() = default;

		virtual void Dump(); // Dump the AST
		void generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir);
		std::string get_name()
		{
			return "%" + std::to_string(id);
		}

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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		virtual void _dump() const override
		{
			std::cout << "Identifier {" << name << "}" << std::endl;
		}
	};

	class Declare : public BaseAST
	{
	public:
		MC::IR::VarType type;
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
		MC::IR::VarType type;
		FuncTypeAST(MC::IR::VarType type) : type(type) {}

	private:
		void _dump() const override
		{
			std::cout << "FuncTypeAST { " << (type == MC::IR::VarType::Ptr ? "Ptr" : "Var") << " }" << std::endl;
		}
	};

	class BlockAST : public BaseAST
	{
	public:
		std::vector<std::unique_ptr<Statement>> stmt;
		BlockAST() {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		MC::IR::BinOp op;
		std::unique_ptr<Expression> rhs;
		UnaryExpression(char op_, Expression *rhs) : rhs(std::move(rhs))
		{
			if (op_ == '+')
				op = MC::IR::BinOp::ADD;
			else if (op_ == '-')
				op = MC::IR::BinOp::SUB;
			else if (op_ == '!')
				op = MC::IR::BinOp::NOT;
			else
				assert(false);
		}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "ConditionExpression { ";
			this->ExpressionValue->Dump();
			std::cout << " }";
		}
	};

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

	class FunctionCall : public Expression
	{
	public:
		std::unique_ptr<Identifier> functionName;
		std::unique_ptr<FunctionCallArgList> arg_list;
		FunctionCall(Identifier *functionName, FunctionCallArgList *arg_list) : functionName(std::move(functionName)), arg_list(std::move(arg_list)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "FunctionCall { ";
			functionName->Dump();
			arg_list->Dump();
			std::cout << " }";
		}
	};

	class Assignment : public Statement
	{
	public:
		std::unique_ptr<Identifier> ident;
		std::unique_ptr<Expression> exp;
		Assignment(Identifier *ident, Expression *exp) : ident(std::move(ident)), exp(std::move(exp)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "Assignment { ";
			ident->Dump();
			exp->Dump();
			std::cout << " }";
		}
	};
	// AfterInc

	class IfElseStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> if_statement;
		std::unique_ptr<Statement> else_statement;
		IfElseStatement(Expression *condition, Statement *if_statement, Statement *else_statement) : condition(std::move(condition)), if_statement(std::move(if_statement)), else_statement(std::move(else_statement)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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

	class WhileStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> stmt;
		WhileStatement(Expression *condition, Statement *stmt) : condition(std::move(condition)), stmt(std::move(stmt)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "WhileStatement { condition(";
			condition->Dump();
			std::cout << " ) ";
			stmt->Dump();
			std::cout << " }";
		}
	};

	class BreakStatement : public Statement
	// ForStatement
	{
	public:
		BreakStatement() {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "BreakStatement { }";
		}
	};

	class ContinueStatement : public Statement
	{
	public:
		ContinueStatement() {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "BreakStatement { }";
		}
	};

	class EvaluateStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> value;
		EvaluateStatement(Expression *value) : value(std::move(value)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		// ReturnStatement(int number) : exp(std::move(new NumberAST(number))) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		std::unique_ptr<FuncTypeAST> type;
		std::vector<std::unique_ptr<Declare>> list;
		DeclareStatement(FuncTypeAST *type) : type(type){};

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
		}
	};

	class FunctionDefineArg : public Expression
	{
	public:
		std::unique_ptr<FuncTypeAST> type;
		std::unique_ptr<Identifier> name;
		FunctionDefineArg(FuncTypeAST *type, Identifier *name) : type(type), name(std::move(name)) {}

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
		std::unique_ptr<FuncTypeAST> func_type;
		std::unique_ptr<Identifier> name;
		std::unique_ptr<FunctionDefineArgList> arg_list;
		std::unique_ptr<BlockAST> block;
		FunctionDefine(FuncTypeAST *return_type, Identifier *name, FunctionDefineArgList *arg_list, BlockAST *block) : func_type(return_type), name(std::move(name)), arg_list(std::move(arg_list)), block(std::move(block)) {}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
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