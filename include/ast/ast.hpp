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
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override
		{
			std::cout << "CompUnitAST {" << std::endl;
			func_def->Dump();
			_printTabs(-1);
			std::cout << "}" << std::endl;
		}
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
		std::vector<BaseAST *> stmt;
		BlockAST() {}
		BlockAST(const BlockAST &c)
		{
			for (auto &i : c.stmt)
			{
				this->stmt.push_back(i);
			}
		}

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

	class ArrayDeclareInitValue : public Expression
	{
	public:
		bool is_number;
		std::unique_ptr<Expression> value;
		std::vector<std::unique_ptr<ArrayDeclareInitValue>> value_list;
		ArrayDeclareInitValue(bool is_number, Expression *value) : is_number(is_number), value(std::move(value)){};
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

	class DeclareStatement : public Statement
	{
	public:
		int type;
		std::vector<Declare *> list;
		DeclareStatement(int type) : type(type){};

	private:
		// ~DeclareStatement() {}
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

	class VarDeclare : public Declare
	{
	public:
		std::unique_ptr<std::string> name;
		VarDeclare(std::string *name) : name(std::move(name)){};

	private:
		void _dump() const override
		{
		}
	};

	class VarDeclareWithInit : public Declare
	{
	public:
		std::unique_ptr<std::string> name;
		std::unique_ptr<Expression> init_value;
		bool is_const;
		VarDeclareWithInit(std::string *name, Expression *init_value, bool is_const = false)
			: name(std::move(name)), init_value(std::move(init_value)), is_const(is_const){};

	private:
		void _dump() const override
		{
		}
	};

}