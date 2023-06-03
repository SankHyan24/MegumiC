#include <optimizer/opt.hpp>

namespace MC::ASM::OPT
{
    std::string Optimizer::executeOptimizer(std::string code)
    {
        return this->_executeOptimizer();
    }

    std::string Optimizer::_executeOptimizer()
    {
        throw std::runtime_error("Optimizer::_executeOptimizer() should not be called");
        return "";
    }

    std::string OptimizerPipeline::executeOptimizers()
    {
        return "";
    }
}