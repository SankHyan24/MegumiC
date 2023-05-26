#include <assembly/generate.hpp>
#include <assembly/irt.hpp>
#include <ir/ir.hpp>

namespace MC::ASM
{
    std::unique_ptr<AssemblyList> generate(std::unique_ptr<MC::IR::IRListWrapper> &irs, std::ostream &out)
    {
        AssemblyList assemblyList(irs);
        assemblyList.Generate();
        return std::make_unique<AssemblyList>(std::move(assemblyList));
    }
}