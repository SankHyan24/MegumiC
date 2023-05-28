#pragma once
#include <memory>
#include <sstream>
#include <vector>

#include <ir/ir.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>

namespace MC::ASM
{

    // AssemblyList is read the ircode from the ircodelist, then generate the ir tree by the generate method
    // after that, generate the target code from the ir tree
    class AssemblyList
    {
    public:
        // input
        std::unique_ptr<MC::IR::IRListWrapper> &irs;
        std::string ir_code;
        // inter
        std::unique_ptr<IRTRoot> irt;
        // output
        std::stringstream buffer;

        AssemblyList(std::unique_ptr<MC::IR::IRListWrapper> &irs);
        void Generate();
        void generateIRTreeFromIRCodeList();
        void Dump(std::ostream &out = std::cout);

    private:
        IRTFunction *_IRT_generate_function(int &codeindex);
        IRTBasicBlock *_IRT_generate_basicblock(int &codeindex);
        IRTInst *_IRT_generate_inst(int &codeindex);
    };

}