#include <assembly/asm.hpp>
#include <ostream>

namespace
{
    inline std::string get_varname_from_str(std::string str)
    {
        if (str.at(0) == '%')
            return "var_" + str.substr(1, str.size());
        if (str.at(0) == '@') // args
            return str.substr(1, str.size());
        else
            return str;
    }

    inline std::string get_label_from_str(std::string str)
    {
        if (str.at(0) == '%')
            return str.substr(1, str.size());
        else if (str.at(0) == '@')
            throw std::runtime_error("Error Label name: " + str + " should not start with @");
        else
            return str;
    }

    inline bool if_number(std::string str)
    {
        if (str.at(0) == '%')
            return false;
        if (str.at(0) == '@')
            return false;
        return true;
    }
}

namespace MC::ASM
{

    AssemblyList::AssemblyList(std::unique_ptr<MC::IR::IRListWrapper> &irs) : irs(irs){};

    void AssemblyList::Generate()
    {
        this->generateIRTreeFromIRCodeList();
        std::ostream out(this->buffer.rdbuf());
        Context ctx;
        this->irt->generate(ctx, out);

        // std::cout << this->buffer.str() << std::endl;
    }

    void AssemblyList::Dump(std::ostream &out)
    {
        out << this->buffer.str();
    }

    void AssemblyList::generateIRTreeFromIRCodeList()
    {
        irt.reset(new IRTRoot());
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        for (int i = 0; i < ir_code_size; i++)
        {
            auto type = ir_code_list->at(i)->IRType;
            if (type == MC::IR::IROp::Void)
                continue;
            else if (type == MC::IR::IROp::FuncDef)
            {
                irt->funcList.push_back(std::unique_ptr<IRTFunction>());
                irt->funcList.back().reset(this->_IRT_generate_function(i));
            }
            else if (type == MC::IR::IROp::GlobalArray || type == MC::IR::IROp::GlobalVar)
            {
                irt->instList.push_back(std::unique_ptr<IRTInst>());
                irt->instList.back().reset(this->_IRT_generate_inst(i));
            }
            else
                throw std::runtime_error("Impossible Scenario: not global or function");
        }
    }

    IRTFunction *AssemblyList::_IRT_generate_function(int &codeindex)
    {
        auto irt_ = new IRTFunction();
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;

        auto fundef_ir_code = dynamic_cast<MC::IR::IRFuncDef *>(ir_code_list->at(codeindex).get());
        if (!fundef_ir_code)
            throw std::runtime_error("expect a funcdef ir but not");
        irt_->retType = (fundef_ir_code->retType == MC::IR::VarType::Ptr) ? MC::ASM::IRArgType::I32Star : MC::ASM::IRArgType::I32;
        irt_->functionName = fundef_ir_code->funcName.substr(1, fundef_ir_code->funcName.size());

        for (auto &i : fundef_ir_code->args)
        {
            MC::ASM::IRArgType argType;
            std::string argName;
            if (i.type == MC::IR::VarType::Ptr)
                argType = MC::ASM::IRArgType::I32Star;
            else
                argType = MC::ASM::IRArgType::I32;
            argName = get_varname_from_str(i.name);
            irt_->argList.push_back(IRArgPair(argType, argName));
        }
        codeindex += 1;
        if (ir_code_list->at(codeindex)->IRType != MC::IR::IROp::Label)
            throw std::runtime_error("Impossible Scenario: function bb head not a lable");
        for (int i = codeindex; i < ir_code_size; i++)
        {
            auto type = ir_code_list->at(i)->IRType;
            if (type == MC::IR::IROp::Label)
            {
                irt_->bbList.push_back(std::unique_ptr<IRTBasicBlock>());
                irt_->bbList.back().reset(this->_IRT_generate_basicblock(i));
            }
            else if (type == MC::IR::IROp::FuncDefEnd)
            {
                codeindex = i + 1;
                return irt_;
            }
            else
            {
                std::cout << ir_code_list->at(i)->dump();
                throw std::runtime_error("Impossible Scenario: bb head not expected");
            }
        }
        codeindex = ir_code_size;
        return irt_;
    }

    IRTBasicBlock *AssemblyList::_IRT_generate_basicblock(int &codeindex)
    {
        auto irt_ = new IRTBasicBlock();
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        if (ir_code_list->at(codeindex)->IRType != MC::IR::IROp::Label)
            throw std::runtime_error("Impossible Scenario: bb head not a lable");
        auto label = dynamic_cast<MC::IR::IRLabel *>(ir_code_list->at(codeindex).get());
        if (!label)
            throw std::runtime_error("label can not be a label ir code");
        irt_->label = label->labelName.substr(1, label->labelName.size());
        codeindex += 1;
        std::cout << "irt_ label is " << irt_->label << std::endl;
        for (int i = codeindex; i < ir_code_size; i++)
        {
            auto type = ir_code_list->at(i)->IRType;
            if (type == MC::IR::IROp::FuncDef || type == MC::IR::IROp::FuncDefEnd)
                throw std::runtime_error("Impossible Scenario: function def/end in a bb");
            if (type == MC::IR::IROp::Void)
            {
                std::cout << "void " << std::endl;
                codeindex = i;
                return irt_;
            }
            // other scenario
            irt_->instLists.push_back(std::unique_ptr<IRTInst>());
            irt_->instLists.back().reset(this->_IRT_generate_inst(i));
        }
        codeindex = ir_code_size;
        return nullptr;
    }

    IRTInst *AssemblyList::_IRT_generate_inst(int &codeindex)
    {
        auto irt_ = new IRTInst();
        auto &ir_code = irs->irList->at(codeindex);
        auto type = ir_code->IRType;
        if (type == MC::IR::IROp::Label || type == MC::IR::IROp::Void || type == MC::IR::IROp::FuncDef || type == MC::IR::IROp::FuncDefEnd)
            throw std::runtime_error("Impossible Scenario: impossible ir type of inst");
        // todo : create ir inst here
        if (ir_code->if_new_var)
            irt_->is_new_var = true;
        irt_->tag = type;
        irt_->line = codeindex;
        irt_->ircode_dst = ir_code->dst;
        switch (type)
        {
        case MC::IR::IROp::Call:
        {
            auto this_ir = dynamic_cast<MC::IR::IRCall *>(ir_code.get());
            irt_->is_call = true;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->opname2 = get_varname_from_str(this_ir->FuncName);
            for (auto &i : this_ir->Args)
                irt_->params_name.push_back(get_varname_from_str(i));
            break;
        }
        /* code */
        case MC::IR::IROp::AssignBinOp:
        {
            auto this_ir = dynamic_cast<MC::IR::IRAssignBinOp *>(ir_code.get());
            irt_->is_2_op = false;
            irt_->opcode = this_ir->op;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->opname2 = get_varname_from_str(this_ir->LHS);
            irt_->opname3 = get_varname_from_str(this_ir->RHS);
            break;
        }
        case MC::IR::IROp::AssignUnaryOp:
        {
            auto this_ir = dynamic_cast<MC::IR::IRAssignUnaryOp *>(ir_code.get());
            irt_->is_2_op = true;
            // irt_
            irt_->opcode = this_ir->op;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->opname2 = get_varname_from_str(this_ir->RHS);
            break;
        }
        case MC::IR::IROp::AssignImm:
        {
            auto this_ir = dynamic_cast<MC::IR::IRAssignImm *>(ir_code.get());
            irt_->is_2_op = true;
            irt_->is_imm = true;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->imm = this_ir->Imm;
            break;
        }
        case MC::IR::IROp::Branch:
        {
            auto this_ir = dynamic_cast<MC::IR::IRBranch *>(ir_code.get());
            irt_->is_branch = true;
            irt_->opname1 = get_varname_from_str(this_ir->Cond);
            irt_->opname2 = get_label_from_str(this_ir->IfLabel);
            irt_->opname3 = get_label_from_str(this_ir->ElseLabel);
            break;
        }
        case MC::IR::IROp::Jump:
        {
            auto this_ir = dynamic_cast<MC::IR::IRJump *>(ir_code.get());
            irt_->is_jump = true;
            irt_->opname1 = get_label_from_str(this_ir->Label);
            break;
        }
        case MC::IR::IROp::GetPtr:
        {
            auto this_ir = dynamic_cast<MC::IR::IRGetPtr *>(ir_code.get());
            irt_->is_getptr = true;
            irt_->getelementptrLvl = this_ir->Lvl;
            irt_->getelementptrType = this_ir->IRGetElementPtrType;

            irt_->is_store_imm = if_number(this_ir->Index);
            irt_->opname1 = get_varname_from_str(this_ir->Ptr);
            irt_->opname2 = get_varname_from_str(this_ir->Var);
            irt_->opname3 = get_varname_from_str(this_ir->Index);

            if (irt_->is_store_imm) // store 0,%1
            {
                irt_->imm = std::stoi(this_ir->Index);
                irt_->opname3 = "imm_" + std::to_string(irt_->imm);
            }

            break;
        }
        case MC::IR::IROp::GetElementPtr:
        {
            auto this_ir = dynamic_cast<MC::IR::IRGetElementPtr *>(ir_code.get());
            irt_->is_getelementptr = true;
            irt_->getelementptrLvl = this_ir->Lvl;
            irt_->getelementptrType = this_ir->IRGetElementPtrType;

            irt_->is_store_imm = if_number(this_ir->Ind);
            irt_->opname1 = get_varname_from_str(this_ir->Ptr);
            irt_->opname2 = get_varname_from_str(this_ir->Arr);
            irt_->opname3 = get_varname_from_str(this_ir->Ind);
            if (irt_->is_store_imm) // store 0,%1
            {
                irt_->imm = std::stoi(this_ir->Ind);
                irt_->opname3 = "imm_" + std::to_string(irt_->imm);
            }
            break;
        }
        case MC::IR::IROp::ArrayDef:
        {
            auto this_ir = dynamic_cast<MC::IR::IRArrayDef *>(ir_code.get());
            irt_->is_array = true;
            // alloc type not need
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->array_shape = this_ir->Shape;
            break;
        }
        case MC::IR::IROp::Store:
        {
            auto this_ir = dynamic_cast<MC::IR::IRStore *>(ir_code.get());
            irt_->is_store = true;
            irt_->is_store_imm = if_number(this_ir->Value);
            irt_->opname1 = get_varname_from_str(this_ir->Value);
            if (irt_->is_store_imm) // store 0,%1
            {
                irt_->imm = std::stoi(this_ir->Value);
                irt_->opname1 = "imm_" + std::to_string(irt_->imm);
            }
            irt_->opname2 = get_varname_from_str(this_ir->AddressOfTarget);
            break;
        }
        case MC::IR::IROp::Load:
        {
            auto this_ir = dynamic_cast<MC::IR::IRLoad *>(ir_code.get());
            irt_->is_load = true;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->opname2 = get_varname_from_str(this_ir->Ptr);
            break;
        }
        case MC::IR::IROp::Alloc:
        {
            auto this_ir = dynamic_cast<MC::IR::IRAlloc *>(ir_code.get());
            irt_->is_alloc = true;
            irt_->is_alloc_array = (this_ir->AllocType == MC::IR::VarType::Ptr);

            irt_->opname1 = get_varname_from_str(this_ir->Var);
            break;
        }
        case MC::IR::IROp::Ret:
        {
            auto this_ir = dynamic_cast<MC::IR::IRRet *>(ir_code.get());
            irt_->is_ret = true;
            irt_->opname1 = get_varname_from_str(this_ir->retVar);
            break;
        }
        case MC::IR::IROp::GlobalVar:
        {
            auto this_ir = dynamic_cast<MC::IR::IRGlobalVar *>(ir_code.get());
            irt_->is_global = true;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->opname2 = get_varname_from_str(this_ir->InitList);
            break;
        }
        case MC::IR::IROp::GlobalArray:
        {
            auto this_ir = dynamic_cast<MC::IR::IRGlobalArray *>(ir_code.get());
            irt_->is_global = true;
            irt_->is_array = true;
            irt_->opname1 = get_varname_from_str(this_ir->Var);
            irt_->array_shape = this_ir->Shape;
            irt_->array_init_buffer = this_ir->initbuffer;
            break;
        }
        default:
            throw std::runtime_error("Unexpected ir inst type");
            break;
        }

        return irt_;
    }
}