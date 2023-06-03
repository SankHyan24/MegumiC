#pragma once
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>

#include <config.hpp>
#include <ir/ir.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>
#include <optimizer/opt.hpp>

namespace MC::OPT
{
    class AsmFile
    {
    public:
        AsmFile(std::string asm_code);
        void deleteComments();
        void addOptimizer(Optimizer *optimizer);
        void Generate();
        void Dump(std::ostream &out = std::cout);

    private:
        int line_count = 0;
        std::vector<AsmCode> codes;
        std::unique_ptr<OptimizerPipeline> optimizer_pipeline{std::make_unique<OptimizerPipeline>()};
    };

    std::unique_ptr<AsmFile> generate(std::string code, MC::config::OptMode optmode);
}