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
		virtual ~BaseAST() = default;
		virtual void Dump() const = 0;
		void generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir);

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir);
	};

	class Expression : public BaseAST
	{
	};

	class Statement : public Expression
	{
	};

	class Declare : public BaseAST
	{
	};

	class CompUnitAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> func_def;
		void Dump() const override
		{
			std::cout << "CompUnitAST { ";
			func_def->Dump();
			std::cout << " }" << std::endl;
		}

	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
	};

	class FuncDefAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> func_type;
		std::string ident;
		std::unique_ptr<BaseAST> block;
		void Dump() const override
		{
			std::cout << "FuncDefAST { ";
			func_type->Dump();
			std::cout << ", " << ident << ", ";
			block->Dump();
			std::cout << " }";
		}
	};

	class FuncTypeAST : public BaseAST
	{
	public:
		std::string type_name;
		void Dump() const override
		{
			std::cout << "FuncTypeAST { " << type_name << " }";
		}
	};

	class BlockAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> stmt;
		void Dump() const override
		{
			std::cout << "BlockAST { ";
			stmt->Dump();
			std::cout << " }";
		}
	};

	class StmtAST : public BaseAST
	{
	public:
		std::unique_ptr<BaseAST> number;
		void Dump() const override
		{
			std::cout << "Stmt { ";
			number->Dump();
			std::cout << " }";
		}
	};

	class NumberAST : public Expression
	{
	public:
		int number;
		void Dump() const override
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
		void Dump() const override
		{
			std::cout << " binE( ";
			this->lhs->Dump();
			std::cout << " " << this->op << " ";
			this->rhs->Dump();
			std::cout << " ) ";
		}
	};

	// class PrimaryExpression : public Expression
	// {
	// public:
	// };

	class UnaryExpression : public Expression
	{
	public:
		char op;
		std::unique_ptr<Expression> rhs;
		UnaryExpression(int op, Expression *rhs) : op(op), rhs(std::move(rhs)) {}
		void Dump() const override
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
		ReturnStatement() {}
		void Dump() const override
		{
			std::cout << "ReturnStatement { ";
			exp->Dump();
			std::cout << " }";
		}
	};
}