#pragma once
#include <ostream>
#include <memory>

#include <ir/ir.hpp>
#include <assembly/asm.hpp>

namespace MC::ASM
{
    std::unique_ptr<AssemblyList> generate(MC::IR::IRListWrapper &irs);
    std::unique_ptr<AssemblyList> generate(MC::IR::IRListWrapper &irs, std::ostream &out);
    int add(int a, int b);
}