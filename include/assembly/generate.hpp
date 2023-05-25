#pragma once
#include <ostream>
#include <memory>

#include <ir/ir.hpp>
#include <assembly/asm.hpp>

namespace MC::ASM
{
    std::unique_ptr<AssemblyList> generate(std::unique_ptr<MC::IR::IRListWrapper> &irs, std::ostream &out = std::cout);
}