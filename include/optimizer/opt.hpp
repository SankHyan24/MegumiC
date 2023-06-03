// Use Peehole optimizer to optimize the Target code
#include <iostream>
#include <vector>
#pragma once
#include <memory>

namespace MC::ASM::OPT
{

    class Optimizer
    {
    public:
        std::string pre_opt_code;
        std::string after_opt_code;
        std::string executeOptimizer(std::string code);

    private:
        virtual std::string _executeOptimizer();
    };

    class PeepholeOptimizer : public Optimizer
    {
    public:
    private:
        virtual std::string _executeOptimizer() override;
    };

    class OptimizerPipeline
    {
    public:
        OptimizerPipeline(){};
        void addOptimizer(Optimizer *optimizer);
        std::string executeOptimizers();

    private:
        std::vector<std::unique_ptr<Optimizer>> opt_pipline;
        std::string pre_opt_code;
        std::string after_opt_code;
    };

}