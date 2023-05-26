#pragma once
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <bitset>

// #include <assembly/asm.hpp>

namespace MC::ASM
{
    class RV32RegUtil
    {
    public:
        static int reg_count;
        static std::string get_x_name(int i);
    };

    class RV32RegBitMask
    {
    public:
        static constexpr int reg_count = 32;
        static constexpr int reg_size = 4;
        std::bitset<reg_count> mask;
        void add_reg(int x);
        void remove_reg(int x);
        bool is_true(int x) const { return mask.test(x); }
        int get_reg_count() const { return mask.count(); }
    };

    class Address
    {
    public:
        int sp_offset;
        Address(int offset = 0) : sp_offset(offset) {}
        int get_offset() const { return sp_offset; }
    };
    class Context
    {
    public:
        static constexpr int reg_count = 12;
        std::ostream &log_out;
        std::vector<std::unordered_map<std::string, Address>> var_table{{}}; // use this structure means that may we can define function in a function
        Context(std::ostream &out = std::cout) : log_out(out)
        {
            var_table.clear();
            var_table.push_back({});
            regs_to_be_protected.clear();
            regs_to_be_protected.push_back(RV32RegBitMask());
            // protect a0-a7
            for (int i = 10; i <= 17; i++)
                regs_to_be_protected.back().add_reg(i);
        }

        void insert_symbol(std::string name, Address value);
        Address &find_symbol_last_table(std::string name);
        void create_scope(int stack_size);
        void end_scope();
        Address allocate_address(int size);
        RV32RegBitMask &getBackBitMask() { return regs_to_be_protected.back(); }

    private:
        std::vector<RV32RegBitMask> regs_to_be_protected; // protect them when call other function
        std::vector<int> stack_allocate_ptr;
        std::vector<int> stack_max_size;
    };
}