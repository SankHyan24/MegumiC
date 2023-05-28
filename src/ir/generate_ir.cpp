#include <cassert>
#include <exception>
#include <unordered_set>

#include <ast/ast.hpp>
#include <config.hpp>
#include <ir/ir.hpp>

using namespace MC::IR;

namespace
{
    int get_number(MC::AST::node::Expression *exp)
    {
        auto numberAST = dynamic_cast<MC::AST::node::NumberAST *>(exp);
        if (!numberAST)
            throw std::runtime_error("Not a number");
        return numberAST->number;
    }

    std::vector<int> get_shape(MC::AST::node::Identifier *identifier)
    {
        auto arrayIdentifier = dynamic_cast<MC::AST::node::ArrayIdentifier *>(identifier);
        if (!arrayIdentifier)
            throw std::runtime_error("Not a array identifier");
        std::vector<int> shape;
        for (auto &i : arrayIdentifier->index_list)
            shape.push_back(get_number(i.get()));
        return shape;
    }

    std::string complete_zeros(std::vector<int> shape, std::vector<int> &init_buffer)
    {
        std::string str;
        std::vector<int> shape_;
        for (int i = 1; i < shape.size(); i++)
            shape_.push_back(shape[i]);
        if (shape.size() == 0)
            return "0";
        for (int i = 0; i < shape[0]; i++)
        {
            if (i == 0)
                str += "{";
            else
                str += ", ";
            if (shape.size() == 1)
            {
                str += "0";
                init_buffer.push_back(0);
            }
            else
                str += complete_zeros(shape_, init_buffer);
            if (i == shape[0] - 1)
                str += "}";
        }
        return str;
    }

    std::string initValue_to_string(MC::AST::node::Expression *exp, std::vector<int> shape, std::vector<int> &init_buffer)
    {
        std::string str;
        auto initValue = dynamic_cast<MC::AST::node::ArrayDeclareInitValue *>(exp);
        std::vector<int> shape_;
        for (int i = 1; i < shape.size(); i++)
            shape_.push_back(shape[i]);
        if (initValue->value == nullptr) // is a value list
        {
            str += "{";
            for (int i = 0; i < shape[0]; i++)
            {
                if (i >= initValue->value_list.size())
                {
                    str += complete_zeros(shape_, init_buffer);
                }
                else
                    str += initValue_to_string(initValue->value_list.at(i).get(), shape_, init_buffer);

                if (i != shape[0] - 1)
                    str += ", ";
            }
            str += "}";
        }
        else
        {
            str += std::to_string(get_number(initValue->value.get()));
            init_buffer.push_back(get_number(initValue->value.get()));
        }
        return str;
    }

    void initZero_to_ir(std::string ir_value_id, int index, std::vector<int> shape, MC::IR::Context &ctx, MC::IR::IRList &ir, bool first_call)
    {
        std::vector<int> shape_;
        for (int i = 1; i < shape.size(); i++)
            shape_.push_back(shape[i]);
        if (shape.size() == 0)
        {
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore("0", ir_value_id));
            return;
        }
        for (int i = 0; i < shape[0]; i++)
        {
            ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
            ir.back().reset(new MC::IR::IRGetElementPtr(ir_value_id + '.' + std::to_string(i), ir_value_id, std::to_string(i), first_call ? 0 : 1, shape.size()));
            initZero_to_ir(ir_value_id + "." + std::to_string(i), i, shape_, ctx, ir, false);
        }
    }

    void initValue_to_ir(std::string ir_value_id, int index, MC::AST::node::Expression *exp, std::vector<int> shape, MC::IR::Context &ctx, MC::IR::IRList &ir, bool first_call)
    {
        auto initValue = dynamic_cast<MC::AST::node::ArrayDeclareInitValue *>(exp);
        std::vector<int> shape_;
        for (int i = 1; i < shape.size(); i++)
            shape_.push_back(shape[i]);

        if (initValue->value == nullptr) // is a value list
        {

            for (int i = 0; i < shape[0]; i++)
            {
                ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
                ir.back().reset(new MC::IR::IRGetElementPtr(ir_value_id + '.' + std::to_string(i), ir_value_id, std::to_string(i), first_call ? 0 : 1, shape.size()));
                if (i >= initValue->value_list.size())
                    initZero_to_ir(ir_value_id + "." + std::to_string(i), i, shape_, ctx, ir, false);
                else
                    initValue_to_ir(ir_value_id + "." + std::to_string(i), i, initValue->value_list.at(i).get(), shape_, ctx, ir, false);
            }
        }
        else
        {
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore(std::to_string(get_number(initValue->value.get())), ir_value_id));
        }
    }

}

namespace MC::AST::node
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
    { // only as right value
        if (dynamic_cast<ArrayIdentifier *>(this))
        {
            dynamic_cast<ArrayIdentifier *>(this)->generate_ir(ctx, ir);
        }
        else
        {
            if (ctx.if_const_exist(this->name)) // TODO:先判断一个作用域里面有没有var再来这里判断const存不存在
            {
                auto &constinfo = ctx.find_const(this->name);
                if (constinfo.is_array)
                    throw std::runtime_error("Can't fetch the value from a array.");
                int value = constinfo.value[0];
                int exp_id = ctx.get_id();
                std::string ir_name_val = "%" + std::to_string(exp_id);
                this->id = exp_id;

                return;
            }

            int exp_id = ctx.get_id();
            std::string ir_name_val = "%" + std::to_string(exp_id);
            this->id = exp_id;

            auto &varinfo = ctx.find_symbol(this->name);
            std::string ir_name_ptr = varinfo.name;
            if (varinfo.is_array)
            {
                auto &shape = varinfo.shape;
                ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
                ir.back().reset(new MC::IR::IRGetElementPtr(ir_name_val, ir_name_ptr, "0", 0, shape.size()));
            }
            else if (varinfo.type == MC::IR::VarType::Ptr)
            {
                ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
                ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
                // ir.push_back(std::unique_ptr<MC::IR::IRGetPtr>());
                // ir.back().reset(new MC::IR::IRGetPtr(ir_name_val, ir_name_ptr));
            }
            else
            {
                ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
                ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
            }
        }
    }

    void ArrayIdentifier::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    { // as right value
        int a_id = ctx.get_id();
        std::string ir_name_val = "%" + std::to_string(a_id);
        this->id = a_id;

        auto &varinfo = ctx.find_symbol(this->name);
        std::string ir_name_ptr = varinfo.name;
        if (!varinfo.is_array) // int a; b = **a[12]**;
        {
            throw std::runtime_error("Can't fetch the value from a non-array.");
        }
        else // int a[100]; b = **a[1]**;
        {
            std::vector<int> shape = varinfo.shape;
            int index_size = this->index_list.size();

            // if (shape.size() != index_size)
            //     throw std::runtime_error("Index number is not match.");

            // int ptr_id = ctx.get_id();
            // std::string ptr_name = "%" + std::to_string(ptr_id);

            // ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
            // ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ptr_name));
            // ir_name_ptr = ptr_name;
            for (int i = 0; i < shape.size(); i++)
            {

                this->index_list.at(i)->generate_ir(ctx, ir);
                std::string exp_ir_name = this->index_list.at(i)->get_name();
                int index_name = ctx.get_id();
                std::string ptr_ir_name = ir_name_ptr + '.' + std::to_string(index_name);
                if (i == 0 && varinfo.type == MC::IR::VarType::Ptr)
                {
                    ir.push_back(std::unique_ptr<MC::IR::IRGetPtr>());
                    ir.back().reset(new MC::IR::IRGetPtr(ptr_ir_name, ir_name_ptr, exp_ir_name));
                }
                else
                {
                    ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
                    ir.back().reset(new MC::IR::IRGetElementPtr(ptr_ir_name, ir_name_ptr, exp_ir_name, i == 0 ? 0 : 1, shape.size() - i));
                }
                ir_name_ptr = ptr_ir_name;
            }

            ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
            ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
        }
    }

    void Assignment::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (dynamic_cast<ArrayIdentifier *>(this->ident.get()))
        {
            // right value
            this->exp->generate_ir(ctx, ir);
            std::string exp_name = this->exp->get_name();

            // left value
            auto arrayIdentifier = dynamic_cast<ArrayIdentifier *>(this->ident.get());

            auto &varinfo = ctx.find_symbol(this->ident->name);
            if (!varinfo.is_array)
                throw std::runtime_error("Can't assign to a variable.");
            std::string ir_name = varinfo.name;

            std::vector<int> shape = varinfo.shape;
            int index_size = arrayIdentifier->index_list.size();
            // if (index_size != shape.size())
            //     throw std::runtime_error("Index number is not match.");

            for (int i = 0; i < shape.size(); i++)
            {
                arrayIdentifier->index_list.at(i)->generate_ir(ctx, ir);
                std::string exp_ir_name = arrayIdentifier->index_list.at(i)->get_name();
                int index_name = ctx.get_id();
                std::string ptr_ir_name = ir_name + "._" + std::to_string(index_name) + "_";
                ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
                ir.back().reset(new MC::IR::IRGetElementPtr(ptr_ir_name, ir_name, exp_ir_name, i == 0 ? 0 : 1, shape.size() - i));
                ir_name = ptr_ir_name;
            }

            // store here
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore(exp_name, ir_name));
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
        if (ctx.if_symbol_exist_in_this_scope(this->name->name))
            throw std::runtime_error("Variable " + this->name->name + " has been declared.");
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
        if (ctx.if_symbol_exist_in_this_scope(this->name->name))
            throw std::runtime_error("Variable " + this->name->name + " has been declared.");
        if (ctx.is_global())
        {
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

            if (this->is_const)
                ctx.insert_const(this->name->name, ConstInfo(get_number(this->init_value.get())));
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

            if (this->is_const)
                ctx.insert_const(this->name->name, ConstInfo(get_number(this->init_value.get())));
        }
    }

    void ArrayDeclare::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (ctx.is_global())
        {
            std::string ir_name = "@" + this->name->name;
            std::vector<int> shape = get_shape(this->name.get());
            ir.push_back(std::unique_ptr<MC::IR::IRGlobalArray>());
            int array_size = 1;
            for (auto i : shape)
                array_size *= i;
            std::vector<int> init_buffer;
            for (int i = 0; i < array_size; i++)
                init_buffer.push_back(0);
            ir.back().reset(new MC::IR::IRGlobalArray(ir_name, this->type, shape, "zeroinit", init_buffer));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name, MC::IR::VarType::Val, true, shape));
        }
        else
        {
            std::string ir_name = "%" + std::to_string(ctx.get_id());
            std::vector<int> shape = get_shape(this->name.get());
            ir.push_back(std::unique_ptr<MC::IR::IRArrayDef>());
            ir.back().reset(new MC::IR::IRArrayDef(ir_name, this->type, shape));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name, MC::IR::VarType::Val, true, shape));
        }
    }

    void ArrayDeclareWithInit::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        if (ctx.is_global())
        {
            std::string ir_name = "@" + this->name->name;
            std::vector<int> shape = get_shape(this->name.get());
            std::vector<int> init_buffer;
            std::string init_value_string = initValue_to_string(this->init_value.get(), shape, init_buffer);

            ir.push_back(std::unique_ptr<MC::IR::IRGlobalArray>());
            ir.back().reset(new MC::IR::IRGlobalArray(ir_name, this->type, shape, init_value_string, init_buffer));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name, MC::IR::VarType::Val, true, shape));
        }
        else
        {
            std::string ir_name = "%" + std::to_string(ctx.get_id());
            std::vector<int> shape = get_shape(this->name.get());
            ir.push_back(std::unique_ptr<MC::IR::IRArrayDef>());
            ir.back().reset(new MC::IR::IRArrayDef(ir_name, this->type, shape));
            ctx.insert_symbol(this->name->name, VarInfo(ir_name, MC::IR::VarType::Val, true, shape));
            initValue_to_ir(ir_name, 0, this->init_value.get(), shape, ctx, ir, true);
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
                std::vector<int> shape = get_shape(arg_identifier);
                // TODO: add shape( no need because the ir cannot recongnize it)
                std::string arg_name = "@arg" + std::to_string(arg_id);
                args.push_back({MC::IR::VarType::Ptr, arg_name});
                args_to_local_name.push_back("%" + std::to_string(arg_id));
                ctx.insert_symbol(i.get()->name.get()->name, MC::IR::VarInfo("%" + std::to_string(arg_id), MC::IR::VarType::Ptr, true, shape));
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

        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel("%" + this->name->name + "_entry"));

        for (int i = 0; i < args.size(); i++)
        {
            ir.push_back(std::unique_ptr<MC::IR::IRAlloc>());
            ir.back().reset(new MC::IR::IRAlloc(args_to_local_name.at(i), args.at(i).type));
            ir.push_back(std::unique_ptr<MC::IR::IRStore>());
            ir.back().reset(new MC::IR::IRStore(args.at(i).name, args_to_local_name.at(i)));
        }

        this->block->generate_ir(ctx, ir);

        if (!dynamic_cast<MC::IR::IRRet *>(ir.back().get()))
        {
            auto number0 = new NumberAST(0);
            auto retStmt = new ReturnStatement(number0);
            retStmt->generate_ir(ctx, ir);
            delete retStmt;
        }
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());

        ir.push_back(std::unique_ptr<MC::IR::IRFuncDefEnd>());
        ir.back().reset(new MC::IR::IRFuncDefEnd());
        ctx.end_scope();
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
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

    void VoidStatement::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) {}

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
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(if_label));
        this->if_statement->generate_ir(ctx, ir);
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(ifelse_end));
        // else block
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(else_label));
        this->else_statement->generate_ir(ctx, ir);
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
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
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(while_start));
        // condition
        this->condition->generate_ir(ctx, ir);
        std::string condition_name = this->condition->get_name();
        // br
        ir.push_back(std::unique_ptr<MC::IR::IRBranch>());
        ir.back().reset(new MC::IR::IRBranch(condition_name, while_loop_body, while_end));
        // while_loop_body
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
        ir.push_back(std::unique_ptr<MC::IR::IRLabel>());
        ir.back().reset(new MC::IR::IRLabel(while_loop_body));
        this->stmt->generate_ir(ctx, ir);
        // jump
        ir.push_back(std::unique_ptr<MC::IR::IRJump>());
        ir.back().reset(new MC::IR::IRJump(while_start));
        // while_end
        ir.push_back(std::unique_ptr<MC::IR::IRVoid>());
        ir.back().reset(new MC::IR::IRVoid());
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