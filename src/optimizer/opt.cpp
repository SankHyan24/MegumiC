#include <optimizer/opt.hpp>
#include <optimizer/asmcode.hpp>
#include <config.hpp>

namespace MC::OPT
{

    void Optimizer::executeOptimizer(std::vector<AsmCode> &pre_opt_codes)
    {
        return this->_executeOptimizer(pre_opt_codes);
    }

    void Optimizer::_executeOptimizer(std::vector<AsmCode> &pre_opt_codes)
    {
        throw std::runtime_error("Optimizer::_executeOptimizer() should not be called");
    }

    void OptimizerPipeline::addOptimizer(Optimizer *optimizer)
    {
        this->opt_pipline.push_back(std::unique_ptr<Optimizer>());
        this->opt_pipline.back().reset(optimizer);
    }

    void OptimizerPipeline::executeOptimizers(std::vector<AsmCode> &pre_opt_codes)
    {
        for (auto &optimizer : this->opt_pipline)
            optimizer->executeOptimizer(pre_opt_codes);
    }

}