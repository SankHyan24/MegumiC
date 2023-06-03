#include <assembly/generate.hpp>
#include <assembly/irt.hpp>
#include <ir/ir.hpp>

namespace
{ // util functions
    using namespace MC::ASM;
    void _load_var_to_reg(Context &ctx, std::ostream &out, std::string varName, int offset, int reg_num)
    {
        try
        {
            Address addr = ctx.find_symbol_last_table(varName);
            int sp_offset = addr.get_offset();
            if (sp_offset < RV32RegUtil::SimmMax)
                out << "\tlw " << RV32RegUtil::get_x_name(reg_num) << ", " << addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << sp_offset << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw " << RV32RegUtil::get_x_name(reg_num) << ", 0(t4)" << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::vector<int> global_shape = ctx.find_global(varName);
            if (global_shape.size() == 0)
                throw std::runtime_error("var not found");
            out << "\tla t0, " << varName << std::endl;
            out << "\tlw " << RV32RegUtil::get_x_name(reg_num) << ", " << offset << "(t0)" << std::endl;
        }
    }

    void _load_var_addr_to_reg(Context &ctx, std::ostream &out, std::string varName, int reg_num)
    {
        try
        {
            Address addr = ctx.find_symbol_last_table(varName);
            if (addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\taddi " << RV32RegUtil::get_x_name(reg_num) << ",sp," << addr.get_offset() << std::endl;
            else
            {
                out << "\tli t4," << addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\taddi " << RV32RegUtil::get_x_name(reg_num) << ",t4,0" << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::vector<int> global_shape = ctx.find_global(varName);
            if (global_shape.size() == 0)
                throw std::runtime_error("var not found");
            out << "\tla " << RV32RegUtil::get_x_name(reg_num) << ", " << varName << std::endl;
        }
    }

    void _store_reg_to_var(Context &ctx, std::ostream &out, std::string varName, int offset, int reg_num)
    {
        if (ctx.if_in_symbol_table(varName))
        {
            Address addr = ctx.find_symbol_last_table(varName);
            int sp_offset = addr.get_offset();
            if (sp_offset < RV32RegUtil::SimmMax)
                out << "\tsw " << RV32RegUtil::get_x_name(reg_num) << ", " << addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << sp_offset << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tsw " << RV32RegUtil::get_x_name(reg_num) << ", 0(t4)" << std::endl;
            }
        }
        else
        {
            std::vector<int> global_shape = ctx.find_global(varName);
            if (global_shape.size() == 0)
                throw std::runtime_error("var not found");
            out << "\tla t0, " << varName << std::endl;
            out << "\tsw " << RV32RegUtil::get_x_name(reg_num) << ", " << offset << "(t0)" << std::endl;
        }
    }

}

namespace MC::ASM
{

    std::unique_ptr<AssemblyList> generate(std::unique_ptr<MC::IR::IRListWrapper> &irs, std::ostream &out)
    {
        AssemblyList assemblyList(irs);
        assemblyList.Generate();
        return std::make_unique<AssemblyList>(std::move(assemblyList));
    }

    void IRTree::_generateLV0(Context &ctx, std::ostream &out)
    {
        throw std::runtime_error("Not implement the IRTree _generate method");
    }
    void IRTInst::_generateLV0(Context &ctx, std::ostream &out)
    {
        out << "\t//" << this->ircode_dst << std::endl;
        // std::cout << "[" << this->line << "]inst is : " << MC::IR::IROp2String(tag) << " " << std::endl;
        // todo
        ctx.insert_type(this->getNewVarName(), this->tag);
        if (this->is_new_var && !this->is_global)
        {
            Address addr = ctx.allocate_address(1);
            ctx.insert_symbol(this->getNewVarName(), addr);
            // out << "//\t insert [" << this->getNewVarName() << "," << MC::IR::IROp2String(tag) << "] into symbol table, at" << std::to_string(addr.get_offset()) << std::endl;
        }
        switch (tag)
        {
        case MC::IR::IROp::Call:
        {
            // protect the register
            int arg_count = this->params_name.size();
            // std::cout << "arg count is " << arg_count << std::endl;
            for (int i = 0, j = 0; i < RV32RegUtil::reg_count && j < arg_count; i++)
                // for (int i = 0, j = 0; i < RV32RegUtil::reg_count; i++)
                if (ctx.getBackBitMask().is_true(i))
                {
                    j++;
                    Address addr = ctx.find_symbol_last_table("reg_" + RV32RegUtil::get_x_name(i));
                    int sp_offset = addr.get_offset();
                    if (sp_offset < RV32RegUtil::SimmMax)
                        out << "\tsw " << RV32RegUtil::get_x_name(i) << ", " << addr.get_offset() << "(sp)" << std::endl;
                    else
                    {
                        out << "\tli t4," << sp_offset << std::endl;
                        out << "\tadd t4, t4, sp" << std::endl;
                        out << "\tsw " << RV32RegUtil::get_x_name(i) << ", 0(t4)" << std::endl;
                    }
                }
            // store the parameters into registers
            for (int i = 0; i < arg_count; i++)
            {
                Address addr = ctx.find_symbol_last_table(this->params_name[i]);
                int sp_offset = addr.get_offset();
                if (sp_offset < RV32RegUtil::SimmMax)
                    out << "\tlw " << RV32RegUtil::get_x_name(i + 10) << ", " << addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << sp_offset << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw " << RV32RegUtil::get_x_name(i + 10) << ", 0(t4)" << std::endl;
                }
            }
            // call
            out << "\tcall " << this->opname2 << std::endl;
            // catch the return value
            Address addr = ctx.find_symbol_last_table(this->getNewVarName());
            if (addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tsw a0, " << addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tsw a0, 0(t4)" << std::endl;
            }
            // restore the register
            for (int i = 0, j = 0; i < RV32RegUtil::reg_count && j < arg_count; i++)
                // for (int i = 0, j = 0; i < RV32RegUtil::reg_count; i++)
                if (ctx.getBackBitMask().is_true(i))
                {
                    j++;
                    Address addr = ctx.find_symbol_last_table("reg_" + RV32RegUtil::get_x_name(i));
                    int sp_offset = addr.get_offset();
                    if (sp_offset < RV32RegUtil::SimmMax)
                        out << "\tlw " << RV32RegUtil::get_x_name(i) << ", " << addr.get_offset() << "(sp)" << std::endl;
                    else
                    {
                        out << "\tli t4," << sp_offset << std::endl;
                        out << "\tadd t4, t4, sp" << std::endl;
                        out << "\tlw " << RV32RegUtil::get_x_name(i) << ", 0(t4)" << std::endl;
                    }
                }

            break;
        }
        case MC::IR::IROp::AssignBinOp:
        { // use t0 t1
            Address op1Addr = ctx.find_symbol_last_table(this->opname1);
            Address op2Addr = ctx.find_symbol_last_table(this->opname2);
            Address op3Addr = ctx.find_symbol_last_table(this->opname3);
            // load t0 op2Addr
            if (op2Addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tlw t0, " << op2Addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << op2Addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw t0, 0(t4)" << std::endl;
            }
            // load t1 op3Addr
            if (op3Addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tlw t1, " << op3Addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << op3Addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw t1, 0(t4)" << std::endl;
            }
            // op
            std::string opEng = MC::IR::BinOp2RV32String(this->opcode);
            // out << "\t" << opEng << " t0, t0, t1" << std::endl;
            switch (this->opcode)
            {
            case MC::IR::BinOp::ADD:
            case MC::IR::BinOp::SUB:
            case MC::IR::BinOp::MUL:
            case MC::IR::BinOp::DIV:
            case MC::IR::BinOp::MOD:
                out << "\t" << opEng << " t0, t0, t1" << std::endl;
                break;
            case MC::IR::BinOp::LT:
                out << "\tslt  t0, t0, t1\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::GT:
                out << "\tsgt  t0, t0, t1\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::LE:
                out << "\tsgt  t0, t0, t1\n";
                out << "\txori t0, t0, 1\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::GE:
                out << "\tslt  t0, t0, t1\n";
                out << "\txori t0, t0, 1\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::EQ:
                out << "\tsub  t0, t0, t1\n";
                out << "\tseqz t0, t0\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::NE:
                out << "\tsub  t0, t0, t1\n";
                out << "\tsnez t0, t0\n";
                out << "\tandi t0, t0, 0xff\n";
                break;
            case MC::IR::BinOp::AND:
                out << "\tsnez  t0, t0\n";
                out << "\tsnez  t1, t1\n";
                out << "\tand  t0, t0, t1\n";
                break;
            case MC::IR::BinOp::OR:
                out << "\tsnez  t0, t0\n";
                out << "\tsnez  t1, t1\n";
                out << "\tor   t0, t0, t1\n";
                break;
            case MC::IR::BinOp::NOT:
                throw("not support not op");
                break;
            }
            // store t0 op1Addr
            if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tsw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << op1Addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tsw t0, 0(t4)" << std::endl;
            }
            break;
        }
        case MC::IR::IROp::AssignUnaryOp:
        {
            Address op1Addr = ctx.find_symbol_last_table(this->opname1);
            Address op2Addr = ctx.find_symbol_last_table(this->opname2);
            // load t0 op2Addr
            // op
            switch (this->opcode)
            {
            case MC::IR::BinOp::NOT:
                if (op2Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t0, " << op2Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op2Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw t0, 0(t4)" << std::endl;
                }
                // t1 = zero -1
                out << "\tli t1, -1" << std::endl;
                out << "\txor t0, t1" << std::endl;
                // store t0 op1Addr
                if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tsw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op1Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tsw t0, 0(t4)" << std::endl;
                }
                break;
            case MC::IR::BinOp::ADD:
                break;
            case MC::IR::BinOp::SUB:
                // t0 =var
                if (op2Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t0, " << op2Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "li t4," << op2Addr.get_offset() << std::endl;
                    out << "add t4, t4, sp" << std::endl;
                    out << "lw t0, 0(t4)" << std::endl;
                }
                // t0 = -t0
                out << "\tsub t0, zero, t0" << std::endl;
                // store t0 op1Addr
                if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tsw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op1Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tsw t0, 0(t4)" << std::endl;
                }
                break;

            default:
                throw std::runtime_error("Not implement the unary op");
                break;
            }
            break;
        }
        case MC::IR::IROp::AssignImm:
        {
            int num = this->imm;
            if (num > -2048 && num < 2047) // 12 bit
            {
                Address op1Addr = ctx.find_symbol_last_table(this->opname1);
                out << "\tli t0, " << num << std::endl;
                if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tsw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op1Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tsw t0, 0(t4)" << std::endl;
                }
            }
            else if (num > -2147483648 && num < 2147483647) // 32 bit
            {
                // use lui
                int high = num >> 12;
                int low = num & 0xfff;
                Address op1Addr = ctx.find_symbol_last_table(this->opname1);
                out << "\tlui t0, " << high << std::endl;
                out << "\taddi t0, t0, " << low << std::endl;
                if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tsw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op1Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tsw t0, 0(t4)" << std::endl;
                }
            }
            else
                throw std::runtime_error("Immediately number is too big, over 32 bit");

            break;
        }
        case MC::IR::IROp::Branch:
        {
            // beq opname1==0, opname3
            // jump opname2
            Address op1Addr = ctx.find_symbol_last_table(this->opname1);
            std::string ifLabel = this->opname2;
            std::string elseLabel = this->opname3;
            if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tlw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << op1Addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw t0, 0(t4)" << std::endl;
            }
            out << "\tbne t0, zero, " << ifLabel << std::endl;
            out << "\tj " << elseLabel << std::endl;
            break;
        }
        case MC::IR::IROp::Jump:
        {
            out << "\tj " << this->opname1 << std::endl;
            break;
        }
        case MC::IR::IROp::GetPtr:
        case MC::IR::IROp::GetElementPtr:
        {

            // %ptr = getelemptr @arr, %index

            // get the address of arr to t0
            if (this->getelementptrType == 0)
                _load_var_addr_to_reg(ctx, out, this->opname2, 5);
            else
                _load_var_to_reg(ctx, out, this->opname2, 0, 5);
            // get the index to t1
            if (this->is_store_imm)
            {
                out << "\tli t1, " << this->imm << std::endl;
            }
            else
                _load_var_to_reg(ctx, out, this->opname3, 0, 6);

            // count the offset to t1
            if (this->getelementptrLvl != 1)
            {
                for (auto &i : this->this_lvl_shape)
                {
                    out << "\tli t3, " << std::to_string(i == 0 ? 1 : i) << std::endl;
                    out << "\tmul t1, t1, t3" << std::endl;
                }
            }
            out << "\tli t2, 4" << std::endl;
            out << "\tmul t1, t1, t2" << std::endl;
            // get the ptr
            out << "\tadd t0, t0, t1" << std::endl;
            // store the ptr to %ptr
            _store_reg_to_var(ctx, out, this->opname1, 0, 5);

            break;
        }
        case MC::IR::IROp::ArrayDef:
        {
            int size = 1;
            for (auto &i : this->array_shape)
                size *= i;
            ctx.allocate_address(size - 1);
            break;
        }
        case MC::IR::IROp::Store:
        {
            // make the value to be saved to t0
            // std::cout << "make the value to be saved to t0" << std::endl;
            if (this->is_store_imm)
            {
                out << "\tli t0, " << this->imm << std::endl;
            }
            else
            {
                Address op1Addr = ctx.find_symbol_last_table(this->opname1);
                MC::IR::IROp op1Type = ctx.find_type(this->opname1);
                if (op1Type == MC::IR::IROp::FuncDef)
                {
                    if (opname1.at(0) != 'a')
                        throw std::runtime_error("Can't store a function referencee");
                    if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                        out << "\tlw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                    else
                    {
                        out << "\tli t4," << op1Addr.get_offset() << std::endl;
                        out << "\tadd t4, t4, sp" << std::endl;
                        out << "\tlw t0, 0(t4)" << std::endl;
                    }
                }
                else
                {
                    if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                        out << "\tlw t0, " << op1Addr.get_offset() << "(sp)" << std::endl;
                    else
                    {
                        out << "\tli t4," << op1Addr.get_offset() << std::endl;
                        out << "\tadd t4, t4, sp" << std::endl;
                        out << "\tlw t0, 0(t4)" << std::endl;
                    }
                }
            }
            // store t0 to opname2
            // std::cout << "store t0 to opname2" << std::endl;
            MC::IR::IROp op2Type = ctx.find_type(this->opname2);
            // std::cout << "type is " << MC::IR::IROp2String(op2Type) << std::endl;
            if (op2Type == MC::IR::IROp::GlobalArray || op2Type == MC::IR::IROp::GlobalVar)
            {
                out << "\tla t3, " << this->opname2 << std::endl;
                out << "\tsw t0, 0(t3)" << std::endl;
            }
            else if (op2Type == MC::IR::IROp::Alloc)
            {
                Address op2Addr = ctx.find_symbol_last_table(this->opname2);
                if (op2Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tsw t0," << op2Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op2Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tsw t0, 0(t4)" << std::endl;
                }
            }
            else if (op2Type == MC::IR::IROp::GetPtr || op2Type == MC::IR::IROp::GetElementPtr)
            {
                Address op2Addr = ctx.find_symbol_last_table(this->opname2);
                if (op2Addr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t1, " << op2Addr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << op2Addr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw t1, 0(t4)" << std::endl;
                }
                out << "\tsw t0, 0(t1)" << std::endl;
            }
            else
            {
                throw std::runtime_error("Can't store other type");
            }
            break;
        }
        case MC::IR::IROp::Load:
        {
            std::string src = this->opname2;
            std::string dst = this->opname1;
            MC::IR::IROp srcType = ctx.find_type(src);
            if (srcType == MC::IR::IROp::GlobalArray || srcType == MC::IR::IROp::GlobalVar)
            {
                out << "\tla t0, " << src << std::endl;
                out << "\tlw t0, 0(t0)" << std::endl;
            }
            else if (srcType == MC::IR::IROp::Alloc && !this->is_alloc_array)
            {
                Address srcAddr = ctx.find_symbol_last_table(src);
                if (srcAddr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t0, " << srcAddr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << srcAddr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw t0, 0(t4)" << std::endl;
                }
            }
            else if (srcType == MC::IR::IROp::Alloc && this->is_alloc_array)
            {
                Address srcAddr = ctx.find_symbol_last_table(src);
                if (srcAddr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t1, " << srcAddr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << srcAddr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw t1, 0(t4)" << std::endl;
                }
                out << "\tlw t0, 0(t1)" << std::endl;
            }
            else if (srcType == MC::IR::IROp::GetPtr || srcType == MC::IR::IROp::GetElementPtr)
            {
                Address srcAddr = ctx.find_symbol_last_table(src);
                if (srcAddr.get_offset() < RV32RegUtil::SimmMax)
                    out << "\tlw t1, " << srcAddr.get_offset() << "(sp)" << std::endl;
                else
                {
                    out << "\tli t4," << srcAddr.get_offset() << std::endl;
                    out << "\tadd t4, t4, sp" << std::endl;
                    out << "\tlw t1, 0(t4)" << std::endl;
                }
                out << "\tlw t0, 0(t1)" << std::endl;
            }
            else
            {
                throw std::runtime_error("Can't load other type");
            }
            Address dstAddr = ctx.find_symbol_last_table(dst);
            if (dstAddr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tsw t0, " << dstAddr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << dstAddr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tsw t0, 0(t4)" << std::endl;
            }
            break;
        }
        case MC::IR::IROp::Alloc:
            break;
        case MC::IR::IROp::Ret:
        {
            // load a0 opname1
            Address op1Addr = ctx.find_symbol_last_table(this->opname1);
            if (op1Addr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tlw a0, " << op1Addr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << op1Addr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw a0, 0(t4)" << std::endl;
            }
            // load ra
            Address raAddr = ctx.find_symbol_last_table("reg_ra");
            if (raAddr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tlw ra, " << raAddr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << raAddr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tlw ra, 0(t4)" << std::endl;
            }
            // restore sp
            if (ctx.this_function_stack_size < RV32RegUtil::SimmMax)
                out << "\taddi sp, sp, " << ctx.this_function_stack_size << std::endl;
            else
            {
                out << "\tli t4," << ctx.this_function_stack_size << std::endl;
                out << "\tadd sp, sp, t4" << std::endl;
            }

            // addi sp, sp, 4
            out << "\tjr ra" << std::endl;
            break;
        }
        case MC::IR::IROp::GlobalVar:
        {
            ctx.insert_global(this->opname1, {1});
            out << this->opname1 << ":" << std::endl;
            if (this->opname2[0] == 'z') // zero init
                out << "\t.zero 4" << std::endl;
            else
                out << "\t.word " << std::stoi(this->opname2) << std::endl;
            break;
        }
        case MC::IR::IROp::GlobalArray:
        {
            auto &shape = this->array_shape;
            auto &name = this->opname1;
            ctx.insert_global(name, shape);
            out << name << ":" << std::endl;
            int zero_accu = 0;
            for (int i = 0; i < this->array_init_buffer.size(); i++)
            {
                if (zero_accu != 0 && this->array_init_buffer[i] != 0)
                {
                    out << "\t.zero " << zero_accu * 4 << std::endl;
                    zero_accu = 0;
                }
                if (this->array_init_buffer[i] == 0)
                    zero_accu++;
                else
                    out << "\t.word " << this->array_init_buffer[i] << std::endl;
            }
            if (zero_accu != 0)
                out << "\t.zero " << zero_accu * 4 << std::endl;
            break;
        }
        default:
            throw std::runtime_error("Unexpected ir inst type");
            break;
        }
        // out << std::endl;
    }
    void IRTBasicBlock::_generateLV0(Context &ctx, std::ostream &out)
    {
        // std::cout << "basic block is : " << label << std::endl;
        // std::cout << "need stack byte is : " << needStackByte << std::endl;

        out << label << ":" << std::endl;
        for (auto &i : instLists)
            i->generate(ctx, out);
    }
    void IRTFunction::_generateLV0(Context &ctx, std::ostream &out)
    {
        // precompute:
        // std::cout << "function is : " << functionName << std::endl;
        needStackByte = 0;
        int needStackByteSR = argList.size() * 4; // Stack and Register(all in S and R=0)
        for (auto &i : bbList)
            needStackByteSR += i->count_need_stack_byte(ctx);
        int needStackByteA = 4 + ctx.getBackBitMask().get_reg_count() * 4; // RA and other register must be saved(a0-7)
        needStackByte = needStackByteSR + needStackByteA;
        // std::cout << "need stack byte is : " << needStackByte << std::endl;
        ctx.this_function_stack_size = needStackByte;
        // label name:
        ctx.create_scope(needStackByte);
        out << "\t.globl\t" << functionName << std::endl;
        out << functionName << ":" << std::endl;
        // prologue:
        out << "\tli t0," << needStackByte << std::endl;
        out << "\tsub sp, sp, t0" << std::endl;
        // save ra
        Address raAddr = ctx.allocate_address(1);
        ctx.insert_symbol("reg_ra", raAddr);
        ctx.insert_type("reg_ra", MC::IR::IROp::FuncDef);
        if (raAddr.get_offset() < RV32RegUtil::SimmMax)
            out << "\tsw ra," << raAddr.get_offset() << "(sp)" << std::endl;
        else
        {
            out << "\tli t4," << raAddr.get_offset() << std::endl;
            out << "\tadd t4, t4, sp" << std::endl;
            out << "\tsw ra,0(t4)" << std::endl;
        }
        // insert other register into symbol table
        for (int i = 0; i < RV32RegUtil::reg_count; i++)
            if (ctx.getBackBitMask().is_true(i))
            {
                Address tmpAddr = ctx.allocate_address(1);
                ctx.insert_symbol("reg_" + RV32RegUtil::get_x_name(i), tmpAddr);
                ctx.insert_type("reg_" + RV32RegUtil::get_x_name(i), MC::IR::IROp::FuncDef);
            }

        // insert the argument into symbol table
        for (int i = 0; i < argList.size(); i++)
        {
            Address tmpAddr = ctx.allocate_address(1);
            ctx.insert_symbol(argList[i].varName, tmpAddr);
            ctx.insert_type(argList[i].varName, MC::IR::IROp::FuncDef);
            int regid;
            if (i < 2)
                regid = i + 10;
            else if (i < 8)
                regid = i + 10;
            else
            {
                throw std::runtime_error("too many arguments");
            }
            if (tmpAddr.get_offset() < RV32RegUtil::SimmMax)
                out << "\tsw " << RV32RegUtil::get_x_name(regid) << "," << tmpAddr.get_offset() << "(sp)" << std::endl;
            else
            {
                out << "\tli t4," << tmpAddr.get_offset() << std::endl;
                out << "\tadd t4, t4, sp" << std::endl;
                out << "\tsw " << RV32RegUtil::get_x_name(regid) << ",0(t4)" << std::endl;
            }
        }
        // function body
        for (auto &i : bbList)
            i->generate(ctx, out);
        // release the stack
        // std::cout << " stack rest :" << ctx.rest_address_size() << std::endl;
        ctx.this_function_stack_size = 0;
        ctx.end_scope();
    }
    void IRTRoot::_generateLV0(Context &ctx, std::ostream &out)
    {
        // std::cout << "generate begin" << std::endl;
        out << "\t.data" << std::endl;
        for (auto &i : instList)
        {
            i->generate(ctx, out);
        }
        out << "\t.text" << std::endl;
        for (auto &i : funcList)
        {
            i->generate(ctx, out);
        }
    }

}