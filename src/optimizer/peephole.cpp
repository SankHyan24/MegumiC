#include <optimizer/peephole.hpp>
#include <optimizer/asmcode.hpp>
#include <config.hpp>

namespace MC::OPT
{
    void PeepholeOptimizer::scanStoreThenLoad(std::vector<AsmCode> &pre_opt_codes)
    {
        for (int i = 0; i < pre_opt_codes.size() - 1; i++)
        {
            AsmCode &code = pre_opt_codes[i];
            AsmCode &next_code = pre_opt_codes[i + 1];
            if (code.getOp() != AsmOp::L_S || next_code.getOp() != AsmOp::L_S)
                continue;
            if (code.getOpStr().compare("sw") || next_code.getOpStr().compare("lw")) // first is load then is store
                continue;
            // first store then load, we can remove load statement
            if (code.getArg1() == next_code.getArg1() && code.getArg2() == next_code.getArg2() && code.getArg3() == next_code.getArg3())
            {
                pre_opt_codes.erase(pre_opt_codes.begin() + i + 1);
                i--;
            }
        }
    }

    void PeepholeOptimizer::_executeOptimizer(std::vector<AsmCode> &pre_opt_codes)
    {
        *MC::config::log << "Using Peephole Optimizer..." << std::endl;
        scanStoreThenLoad(pre_opt_codes);
        *MC::config::log << "Peephole Optimization done" << std::endl;
    }
}