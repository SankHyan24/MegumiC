// Use Peehole optimizer to optimize the Target code
#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <optimizer/asmcode.hpp>

namespace MC::OPT
{

    class Optimizer
    {
    public:
        void executeOptimizer(std::vector<AsmCode> &pre_opt_codes);

    private:
        virtual void _executeOptimizer(std::vector<AsmCode> &pre_opt_codes);
    };

    class OptimizerPipeline
    {
    public:
        OptimizerPipeline(){};
        void addOptimizer(Optimizer *optimizer);
        void executeOptimizers(std::vector<AsmCode> &pre_opt_codes);

    private:
        std::vector<std::unique_ptr<Optimizer>> opt_pipline;
    };

}