#include <assembly/context.hpp>
namespace MC::ASM
{
    int RV32RegUtil::reg_count = 32;
    int RV32RegUtil::SimmMax = 2046;
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
    void Context::insert_function_need_stack(std::string name, int stack_size)
    {
        this->function_need_stack.insert({name, stack_size});
    }
    int Context::find_function_need_stack(std::string name)
    {
        auto it = this->function_need_stack.find(name);
        if (it == this->function_need_stack.end())
            throw std::runtime_error("function[" + name + "] not found in function_need_stack table!");
        return it->second;
    }

    void Context::insert_symbol(std::string name, Address value)
    {
        this->var_table.back().insert({name, value});
    }

    void Context::insert_type(std::string name, MC::IR::IROp type)
    {
        this->var_type.insert({name, type});
    }

    void Context::insert_global(std::string name, std::vector<int> value)
    {
        this->global_table.insert({name, value});
    }
    MC::IR::IROp Context::find_type(std::string name)
    {
        auto it = this->var_type.find(name);
        if (it == this->var_type.end())
            throw std::runtime_error("type[" + name + "] not found in var type table!");
        return it->second;
    }

    Address &Context::find_symbol_last_table(std::string name)
    {
        if (this->var_table.empty())
            throw std::runtime_error("no var table");
        auto &table = this->var_table.back();

        auto it = table.find(name);
        if (it == table.end())
            throw std::runtime_error("symbol[" + name + "] not found in var table table!");
        return it->second;
    }
    bool Context::if_in_symbol_table(std::string name)
    {
        for (auto &table : this->var_table)
        {
            auto it = table.find(name);
            if (it != table.end())
                return true;
        }
        return false;
    }
    std::vector<int> &Context::find_global(std::string name)
    {
        auto it = this->global_table.find(name);
        if (it == this->global_table.end())
            throw std::runtime_error("symbol[" + name + "] not found in var table and global table!");
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
    int Context::rest_address_size()
    {
        int res;
        int ptr = this->stack_allocate_ptr.back();
        int max_size = this->stack_max_size.back();
        return max_size - ptr;
    }

}