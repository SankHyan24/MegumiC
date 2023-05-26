#include <assembly/context.hpp>
namespace MC::ASM
{
    int RV32RegUtil::reg_count = 32;
    std::string RV32RegUtil::get_x_name(int x)
    {
        std::string name("");
        if (x < 0 || x >= reg_count)
            throw std::runtime_error("invalid register");
        if (x == 0)
            return "zero";
        else if (x == 1)
            return "ra";
        else if (x == 2)
            return "sp";
        else if (x == 3)
            return "gp";
        else if (x == 4)
            return "tp";
        else if (x >= 5 && x <= 7)
            return "t" + std::to_string(x - 5);
        else if (x >= 8 && x <= 9)
            return "s" + std::to_string(x - 8); // s0 or fp
        else if (x >= 10 && x <= 17)
            return "a" + std::to_string(x - 10);
        else if (x >= 18 && x <= 27)
            return "s" + std::to_string(x - 16);
        else if (x >= 28 && x <= 31)
            return "t" + std::to_string(x - 24);
        else
            throw std::runtime_error("invalid register");
    }

    void RV32RegBitMask::add_reg(int x)
    {
        if (x < 0 || x >= reg_count)
            throw std::runtime_error("invalid register");
        this->mask.set(x);
    }

    void RV32RegBitMask::remove_reg(int x)
    {
        if (x < 0 || x >= reg_count)
            throw std::runtime_error("invalid register");
        this->mask.reset(x);
    }

    void Context::insert_symbol(std::string name, Address value)
    {
        this->var_table.back().insert({name, value});
    }
    Address &Context::find_symbol_last_table(std::string name)
    {
        if (this->var_table.empty())
            throw std::runtime_error("no var table");
        auto &table = this->var_table.back();

        auto it = table.find(name);
        if (it == table.end())
            throw std::runtime_error("symbol not found");
        return it->second;
    }

    void Context::create_scope(int stack_size)
    {
        this->var_table.push_back({});
        this->stack_max_size.push_back(stack_size);
        this->stack_allocate_ptr.push_back(0);
    }
    void Context::end_scope()
    {
        this->var_table.pop_back();
        this->stack_max_size.pop_back();
        this->stack_allocate_ptr.pop_back();
    }
    Address Context::allocate_address(int argSize)
    {
        int byteSize = argSize * 4;
        if (this->stack_max_size.empty())
            throw std::runtime_error("no address allocator");
        int &ptr = this->stack_allocate_ptr.back();
        int &max_size = this->stack_max_size.back();
        if (ptr + byteSize > max_size)
            throw std::runtime_error("stack overflow");
        int ret = ptr;
        ptr += byteSize;
        return Address(ret);
    }
}