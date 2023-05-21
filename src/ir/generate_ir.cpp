#include <cassert>
#include <exception>
#include <unordered_set>

#include <ast/ast.hpp>
#include <config.hpp>
#include <ir/ir.hpp>

using namespace MC::IR;

namespace MC::ast::node
{
    void BaseAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        throw std::runtime_error("Not implemented");
    }

    void CompUnitAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        for (auto &i : list)
        {
            i->generate_ir(ctx, ir);
        }
    }

    void DeclareStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        for (auto &i : list)
        {
            i->generate_ir(ctx, ir);
        }
    }

    void VarDeclare::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
    }

    void BlockAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        ctx.create_scope();
        for (auto &i : stmt)
            i->generate_ir(ctx, ir);
        ctx.end_scope();
    }

    void FunctionDefine::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        ctx.create_scope();
        // Function define
        std::string functionName = "@" + this->name->name;
        VarType retType = MC::IR::VarType::Val;
        std::vector<MC::IR::ArgPair> args;
        for (auto &i : this->arg_list->list)
        {
            auto arg_identifier = dynamic_cast<ArrayIdentifier *>(i.get()->name.get());
            if (arg_identifier)
            {
                std::vector<int> shape;
                // TODO: add shape
                std::string arg_name = "%" + std::to_string(ctx.get_id());
                ctx.insert_symbol(i.get()->name.get()->name, MC::IR::VarInfo(arg_name));
                args.push_back({MC::IR::VarType::Ptr, arg_name});
            }
            else
            {
                std::string arg_name = "%" + std::to_string(ctx.get_id());
                ctx.insert_symbol(i.get()->name.get()->name, MC::IR::VarInfo(arg_name));
                args.push_back({MC::IR::VarType::Val, arg_name});
            }
        }
        ir.push_back(std::unique_ptr<MC::IR::IRFuncDef>());
        ir.back().reset(new MC::IR::IRFuncDef(functionName, retType, args));

        // TODO:
        this->block->generate_ir(ctx, ir);

        ir.push_back(std::unique_ptr<MC::IR::IRFuncDefEnd>());
        ir.back().reset(new MC::IR::IRFuncDefEnd());
        ctx.end_scope();
    }

    void ReturnStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {

        this->exp->generate_ir(ctx, ir);
        std::string return_name = this->exp->get_name();

        ir.push_back(std::unique_ptr<MC::IR::IRRet>());
        ir.back().reset(new MC::IR::IRRet(return_name));
    }

    void NumberAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        std::string name = "%" + std::to_string(ctx.get_id());
        this->id = ctx.get_last_id();

        ir.push_back(std::unique_ptr<MC::IR::IRAssignImm>());
        ir.back().reset(new MC::IR::IRAssignImm(name, this->number));
    }

    void BinaryExpression::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {

        this->lhs->generate_ir(ctx, ir);
        this->rhs->generate_ir(ctx, ir);

        std::string name = "%" + std::to_string(ctx.get_id());
        this->id = ctx.get_last_id();

        ir.push_back(std::unique_ptr<MC::IR::IRAssignBinOp>());
        ir.back().reset(new MC::IR::IRAssignBinOp(name, this->lhs->get_name(), this->rhs->get_name(), this->op));
    }

    void UnaryExpression::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        std::string name = "%" + std::to_string(ctx.get_id());
        this->id = ctx.get_last_id();

        this->rhs->generate_ir(ctx, ir);
        ir.push_back(std::unique_ptr<MC::IR::IRAssignBinOp>());
        ir.back().reset(new MC::IR::IRAssignUnaryOp(name, this->rhs->get_name(), this->op));
    }
}