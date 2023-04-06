#pragma once
#include <iostream>
#include <memory>
// 所有 AST 的基类
namespace MC::ast
{
  class BaseAST
  {
  public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
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

  class NumberAST : public BaseAST
  {
  public:
    int number;
    void Dump() const override
    {
      std::cout << number;
    }
  };
}