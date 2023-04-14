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

	class Declare : public BaseAST
	{
	};

	class CompUnitAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> func_def;
		CompUnitAST(BaseAST *func_def) : func_def(std::move(func_def)) {}

	private:
		void _dump() const override
		{
			std::cout << "CompUnitAST {" << std::endl;
			func_def->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
	};

	class FuncDefAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> func_type;
		std::unique_ptr<std::string> ident;
		std::unique_ptr<BaseAST> block;
		FuncDefAST(BaseAST *func_type, std::string *ident, BaseAST *block)
			: func_type(std::move(func_type)), ident(std::move(ident)), block(std::move(block)) {}

	private:
		void _dump() const override
		{
			std::cout << "FuncDefAST " << *ident << "()" << std::endl;
			func_type->Dump();
			block->Dump();
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
		std::unique_ptr<BaseAST> stmt;
		BlockAST(BaseAST *stmt) : stmt(std::move(stmt)) {}

	private:
		void _dump() const override
		{
			std::cout << "BlockAST {" << std::endl;
			stmt->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
	};

	class StmtAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> number;

	private:
		void _dump() const override
		{
			std::cout << "Stmt {" << std::endl;
			number->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
	};

	class NumberAST : public Expression
	{
	public:
		int number;
		NumberAST(int number) : number(number) {}

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

	class ReturnStatement : public Statement
	{
	public:
		std::unique_ptr<Expression> exp;
		ReturnStatement(Expression *exp) : exp(std::move(exp)) {}

	private:
		void _dump() const override
		{
			std::cout << "ReturnStatement { ";
			exp->Dump();
			std::cout << " }" << std::endl;
		}
	};
}