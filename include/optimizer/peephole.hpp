#pragma once
#include <optimizer/opt.hpp>
namespace MC::OPT
{
    class PeepholeOptimizer : public Optimizer
    {
    private:
        void scanStoreThenLoad(std::vector<AsmCode> &pre_opt_codes);
        virtual void _executeOptimizer(std::vector<AsmCode> &pre_opt_codes) override;
    };
}