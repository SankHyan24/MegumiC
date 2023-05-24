#include <assembly/generate.hpp>

namespace MC::ASM
{
    std::unique_ptr<AssemblyList> generate(MC::IR::IRListWrapper &irs)
    {
        generate(irs, std::cout);
    }
    std::unique_ptr<AssemblyList> generate(MC::IR::IRListWrapper &irs, std::ostream &out)
    {
    }

    int add(int a, int b)
    {
        return a + b;
    }
}