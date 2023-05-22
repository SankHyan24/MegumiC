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

    void Identifier::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (dynamic_cast<ArrayIdentifier *>(this))
        {
            // dynamic_cast<ArrayIdentifier *>(this->ident.get())->store_runtime(rhs, ctx, ir);
        }
        else
        {
            auto &varinfo = ctx.find_symbol(this->name);
            if (varinfo.is_array)
                throw std::runtime_error("Can't fetch the value from a array.");
            std::string ir_name_ptr = varinfo.name;

            int exp_id = ctx.get_id();
            std::string ir_name_val = "%" + std::to_string(exp_id);
            this->id = exp_id;

            ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
            ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
        }
    }

    void Assignment::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (dynamic_cast<ArrayIdentifier *>(this->ident.get()))
        {
            // dynamic_cast<ArrayIdentifier *>(this->ident.get())->store_runtime(rhs, ctx, ir);
        }
        else
        {
            auto &varinfo = ctx.find_symbol(this->ident->name);
            if (varinfo.is_array)
                throw std::runtime_error("Can't assign to a array.");
            std::string ir_name = varinfo.name;

            this->exp->generate_ir(ctx, ir);
            std::string exp_name = this->exp->get_name();

            // store here
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore(exp_name, ir_name));
        }
    }

    void DeclareStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        for (auto &i : list)
        {
            i->type = this->type->type;
            i->generate_ir(ctx, ir);
        }
    }

    void VarDeclare::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (ctx.is_global())
        {
            std::string ir_name = "@" + this->name->name;
            ir.push_back(std::unique_ptr<MC::IR::IRGlobalVar>());
            ir.back().reset(new MC::IR::IRGlobalVar(ir_name, this->type, "zeroinit"));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name));
        }
        else
        {
            std::string ir_name = "%" + std::to_string(ctx.get_id());
            ir.push_back(std::unique_ptr<MC::IR::IRAlloc>());
            ir.back().reset(new MC::IR::IRAlloc(ir_name, this->type));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name));
        }
    }

    void VarDeclareWithInit::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (ctx.is_global())
        { // TODO:
            std::string ir_name = "@" + this->name->name;
            auto number_casted = dynamic_cast<NumberAST *>(this->init_value.get());
            if (!number_casted)
            {
                throw std::runtime_error("init not a number!");
            }
            std::string init_value_string = std::to_string(number_casted->number);

            ir.push_back(std::unique_ptr<MC::IR::IRGlobalVar>());
            ir.back().reset(new MC::IR::IRGlobalVar(ir_name, this->type, init_value_string));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name));
        }
        else
        {
            std::string ir_name = "%" + std::to_string(ctx.get_id()); //?
            ctx.insert_symbol(this->name->name, VarInfo(ir_name));

            // alloc here:
            ir.push_back(std::unique_ptr<MC::IR::IRAlloc>());
            ir.back().reset(new MC::IR::IRAlloc(ir_name, this->type));

            Assignment assign(this->name.release(), this->init_value.release());
            assign.generate_ir(ctx, ir);

            this->name.reset(assign.ident.release());
            this->init_value.reset(assign.exp.release());
        }
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

        VarType retType = this->func_type->type;
        std::vector<MC::IR::ArgPair> args;
        std::vector<std::string> args_to_local_name;
        for (auto &i : this->arg_list->list)
        {
            auto arg_identifier = dynamic_cast<ArrayIdentifier *>(i.get()->name.get());
            int arg_id = ctx.get_id();
            if (arg_identifier)
            {
                std::vector<int> shape;
                // TODO: add shape( no need because the ir cannot recongnize it)
                std::string arg_name = "@arg" + std::to_string(arg_id);
                args.push_back({MC::IR::VarType::Ptr, arg_name});
                args_to_local_name.push_back("%" + std::to_string(arg_id));
                ctx.insert_symbol(i.get()->name.get()->name, MC::IR::VarInfo("%" + std::to_string(arg_id)));
            }
            else
            {
                std::string arg_name = "@arg" + std::to_string(arg_id);
                args.push_back({MC::IR::VarType::Val, arg_name});
                args_to_local_name.push_back("%" + std::to_string(arg_id));
                ctx.insert_symbol(i.get()->name.get()->name, MC::IR::VarInfo("%" + std::to_string(arg_id)));
            }
        }
        ir.push_back(std::unique_ptr<MC::IR::IRFuncDef>());
        ir.back().reset(new MC::IR::IRFuncDef(functionName, retType, args));

        for (int i = 0; i < args.size(); i++)
        {
            ir.push_back(std::unique_ptr<MC::IR::IRAlloc>());
            ir.back().reset(new MC::IR::IRAlloc(args_to_local_name.at(i), args.at(i).type));
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore(args.at(i).name, args_to_local_name.at(i)));
        }

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

    void EvaluateStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        this->value->generate_ir(ctx, ir);
    }

    void IfElseStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        int if_else_id = ctx.get_id();
        std::string if_label = "%mc_then" + std::to_string(if_else_id);
        std::string else_label = "%mc_else" + std::to_string(if_else_id);
        std::string ifelse_end = "%mc_ifelse_end" + std::to_string(if_else_id);

        // TODO eval condition here (not implemented)
        this->condition->generate_ir(ctx, ir);
        std::string condition_name = this->condition->get_name();
        // br
        ir.push_back(std::unique_ptr<MC::IR::IRBranch>());
        ir.back().reset(new MC::IR::IRBranch(condition_name, if_label, else_label));
        // ifthen block
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(if_label));
        this->if_statement->generate_ir(ctx, ir);
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(ifelse_end));
        // else block
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(else_label));
        this->else_statement->generate_ir(ctx, ir);
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(ifelse_end));
    }

    void WhileStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        // while_start:
        //    condition
        //    br condition while_loop_body while_end
        // while_loop_body:
        //    ...
        //    jump while_start
        // while_end:

        int while_id = ctx.get_id();
        std::string while_start = "%mc_while_start" + std::to_string(while_id);
        std::string while_loop_body = "%mc_while_body" + std::to_string(while_id);
        std::string while_end = "%mc_while_end" + std::to_string(while_id);

        ctx.add_loop_label(while_start);
        ctx.add_loop_label(while_end);

        // label
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(while_start));
        // condition
        this->condition->generate_ir(ctx, ir);
        std::string condition_name = this->condition->get_name();
        // br
        ir.push_back(std::unique_ptr<MC::IR::IRBranch>());
        ir.back().reset(new MC::IR::IRBranch(condition_name, while_loop_body, while_end));
        // while_loop_body
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(while_loop_body));
        this->stmt->generate_ir(ctx, ir);
        // jump
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(while_start));
        // while_end
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(while_end));

        ctx.pop_loop_label();
        ctx.pop_loop_label();
    }

    void BreakStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        std::string break_label = ctx.get_loop_label();
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(break_label));
    }

    void ContinueStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        std::string break_label = ctx.get_loop_label();
        ctx.pop_loop_label();
        std::string continue_label = ctx.get_loop_label();
        ctx.add_loop_label(break_label);
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(continue_label));
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
        this->rhs->generate_ir(ctx, ir);

        std::string name = "%" + std::to_string(ctx.get_id());
        this->id = ctx.get_last_id();

        ir.push_back(std::unique_ptr<MC::IR::IRAssignBinOp>());
        ir.back().reset(new MC::IR::IRAssignUnaryOp(name, this->rhs->get_name(), this->op));
    }

    void ConditionExpression::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        this->ExpressionValue->generate_ir(ctx, ir);
        this->id = this->ExpressionValue->id;
    }

    void FunctionCall::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        std::string funcName = "@" + this->functionName->name;
        std::vector<std::string> args;

        for (auto &i : this->arg_list->arg_list)
        {
            i->generate_ir(ctx, ir);
            args.push_back(i->get_name());
        }

        std::string this_name = "%" + std::to_string(ctx.get_id());
        this->id = ctx.get_last_id();

        ir.push_back(std::unique_ptr<MC::IR::IRCall>());
        ir.back().reset(new MC::IR::IRCall(this_name, funcName, args));
    }
}