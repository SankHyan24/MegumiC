#include <optimizer/generate.hpp>
#include <optimizer/peephole.hpp>

namespace MC::OPT
{
    std::unique_ptr<AsmFile> generate(std::string code, MC::config::OptMode optmode)
    {
        AsmFile asmfile(code);
        if (optmode != MC::config::OptMode::NONE)
        {
            // peephole optimizer
            auto peephole_optimizer = new PeepholeOptimizer();
            asmfile.addOptimizer(peephole_optimizer);
            asmfile.Generate();
        }
        return std::make_unique<AsmFile>(std::move(asmfile));
    }

    AsmFile::AsmFile(std::string asm_code)
    {
        // split the asm_code by "\n"
        std::string::size_type pos = asm_code.find("\n");
        while (pos != std::string::npos)
        {
            std::string line = asm_code.substr(0, pos);
            asm_code.erase(0, pos + 1);
            this->codes.push_back(AsmCode(line));
            pos = asm_code.find("\n");
        }
        // the last line
        if (asm_code.size() != 0)
            this->codes.push_back(AsmCode(asm_code));
        deleteComments();
        line_count = this->codes.size();
    }

    void AsmFile::deleteComments()
    {
        // delete the comments
        for (int i = 0; i < this->codes.size(); i++)
            if (this->codes[i].getOp() == AsmOp::COMMENT)
                this->codes.erase(this->codes.begin() + i);
    }

    void AsmFile::addOptimizer(Optimizer *optimizer)
    {
        this->optimizer_pipeline->addOptimizer(optimizer);
    }

    void AsmFile::Generate()
    {
        this->optimizer_pipeline->executeOptimizers(this->codes);
        int new_count = this->codes.size();
        float rate = (float)((line_count - new_count) * 100) / (float)line_count;
        std::string rate_str = std::to_string(rate);
        rate_str = rate_str.substr(0, rate_str.find(".") + 3);
        rate = std::stof(rate_str);
        std::cout << "Optimization: " << rate << "\% lines deleted." << std::endl;
    }

    void AsmFile::Dump(std::ostream &out)
    {
        for (auto code : this->codes)
            code.Dump(out);
    }
}