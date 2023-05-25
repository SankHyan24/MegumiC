#include <assembly/asm.hpp>

namespace MC::ASM
{

    AssemblyList::AssemblyList(std::unique_ptr<MC::IR::IRListWrapper> &irs) : irs(irs){};

    void AssemblyList::Generate()
    {
    }

    void AssemblyList::generateIRTreeFromIRCodeList()
    {
        irt.reset(new IRTRoot());
        IRTree *treenode = irt.get();
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        for (int i = 0; i < ir_code_size; i++)
        {
            if (ir_code_list->at(i)->IRType == MC::IR::IROp::FuncDef)
            {
                irt->funcList.push_back(std::unique_ptr<IRTFunction>());
                irt->funcList.back().reset(this->_IRT_generate_function(i));
            }
            else if (ir_code_list->at(i)->IRType == MC::IR::IROp::GlobalArray)
            {
                irt->instList.push_back(std::unique_ptr<IRTInst>());
                irt->instList.back().reset(this->_IRT_generate_inst(i));
            }
            else
                throw std::runtime_error("Impossible Scenario");
        }
    }

    void AssemblyList::Dump()
    {
    }

    IRTFunction *AssemblyList::_IRT_generate_function(int &codeindex)
    {
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        if (ir_code_list->at(codeindex)->IRType != MC::IR::IROp::Label)
        {
        }

        for (int i = codeindex; i < ir_code_size; i++)
        {
            // todo
        }
        return nullptr;
    }

    IRTBasicBlock *AssemblyList::_IRT_generate_basicblock(int &codeindex)
    {
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        for (int i = codeindex; i < ir_code_size; i++)
        {
            auto type = ir_code_list->at(i)->IRType;
            if (type == MC::IR::IROp::FuncDef)
                throw std::runtime_error("Impossible Scenario: function def in a B.B.");
            if (type == MC::IR::IROp::Void || type == MC::IR::IROp::FuncDefEnd)
            {
                codeindex = i;
            }
        }
        return nullptr;
    }

    IRTInst *AssemblyList::_IRT_generate_inst(int &codeindex)
    {
        int ir_code_size = irs->irList->size();
        auto &ir_code_list = irs->irList;
        for (int i = codeindex; i < ir_code_size; i++)
        {
        }
        return nullptr;
    }
}