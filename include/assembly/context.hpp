#pragma once
#include <ostream>
#include <string>
#include <unordered_map>

// #include <assembly/asm.hpp>

namespace MC::ASM
{
    class Context
    {
    public:
        static constexpr int reg_count = 12;
        std::ostream &log_out;
    };
}