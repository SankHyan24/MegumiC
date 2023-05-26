#include <assembly/asm.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>
// RV32
namespace MC::ASM
{
    void IRTree::generate(Context &ctx, std::ostream &out) // entrance of irtree to target code generation
    {
        this->_generateLV0(ctx, out);
    }

    int IRTInst::count_need_stack_byte(Context &ctx)
    {
        int byteS = 0;
        switch (tag)
        {
        case MC::IR::IROp::Call:
            byteS += 4; // a=func()  var itself
            break;
        case MC::IR::IROp::AssignBinOp:
            byteS += 4;
            break;
        case MC::IR::IROp::AssignUnaryOp:
            byteS += 4;
            break;
        case MC::IR::IROp::AssignImm:
            byteS += 4;
            break;
        case MC::IR::IROp::Branch:
            break;
        case MC::IR::IROp::Jump:
            break;
        case MC::IR::IROp::GetPtr:
            byteS += 4;
            break;
        case MC::IR::IROp::GetElementPtr:
            byteS += 4;
            break;
        case MC::IR::IROp::ArrayDef:
        {
            int tmp = 1;
            for (auto &i : array_shape)
                tmp *= i;
            byteS += tmp * 4;
            break;
        }
        case MC::IR::IROp::Store:
            break;
        case MC::IR::IROp::Load:
            byteS += 4;
            break;
        case MC::IR::IROp::Alloc:
            byteS += 4;
            break;
        case MC::IR::IROp::Ret:
            break;
        case MC::IR::IROp::GlobalVar:
            byteS += 4;
            break;
        case MC::IR::IROp::GlobalArray:
        {
            int tmp = 1;
            for (auto &i : array_shape)
                tmp *= i;
            byteS += tmp * 4;
            break;
        }
        default:
            throw std::runtime_error("Unexpected ir inst type");
            break;
        }
        return byteS;
    }
    void IRTBasicBlock::_count_need_stack_byte(Context &ctx)
    {
        int byteSR = 0;
        for (auto &i : instLists)
            byteSR += i->count_need_stack_byte(ctx);
        needStackByte = byteSR;
    }
    int IRTBasicBlock::count_need_stack_byte(Context &ctx)
    {
        _count_need_stack_byte(ctx);
        return needStackByte;
    }

    void IRTree::_generateLV0(Context &ctx, std::ostream &out)
    {
        throw std::runtime_error("Not implement the IRTree _generate method");
    }
    void IRTInst::_generateLV0(Context &ctx, std::ostream &out)
    {
        std::cout << "inst is : " << MC::IR::IROp2String(tag) << std::endl;
        // todo
        if (this->is_new_var)
            ctx.insert_symbol(this->getNewVarName(), ctx.allocate_address(1));
        switch (tag)
        {
        case MC::IR::IROp::Call:
        {
            // protect the register
            int arg_count = this->params_name.size();
            std::cout << "arg count is " << arg_count << std::endl;
            for (int i = 0, j = 0; i < RV32RegUtil::reg_count && j < arg_count; i++)
                if (ctx.getBackBitMask().is_true(i))
                {
                    j++;
                    Address addr = ctx.find_symbol_last_table("reg_" + RV32RegUtil::get_x_name(i));
                    out << "\tsw " << RV32RegUtil::get_x_name(i) << ", " << addr.get_offset() << "(sp)" << std::endl;
                }
            // store the parameters into registers

            // call
            // catch the return value
            // restore the register

            break;
        }
        case MC::IR::IROp::AssignBinOp:
        {

            break;
        }
        case MC::IR::IROp::AssignUnaryOp:
        {

            break;
        }
        case MC::IR::IROp::AssignImm:
        {

            break;
        }
        case MC::IR::IROp::Branch:
        {

            break;
        }
        case MC::IR::IROp::Jump:
        {

            break;
        }
        case MC::IR::IROp::GetPtr:
        {

            break;
        }
        case MC::IR::IROp::GetElementPtr:
        {

            break;
        }
        case MC::IR::IROp::ArrayDef:
        {

            break;
        }
        case MC::IR::IROp::Store:
        {
        }
        case MC::IR::IROp::Load:
        {

            break;
        }
        case MC::IR::IROp::Alloc:
        {

            break;
        }
        case MC::IR::IROp::Ret:
        {

            break;
        }
        case MC::IR::IROp::GlobalVar:
        {

            break;
        }
        case MC::IR::IROp::GlobalArray:
        {

            break;
        }
        default:
            throw std::runtime_error("Unexpected ir inst type");
            break;
        }
    }
    void IRTBasicBlock::_generateLV0(Context &ctx, std::ostream &out)
    {
        std::cout << "basic block is : " << label << std::endl;
        std::cout << "need stack byte is : " << needStackByte << std::endl;

        out << label << ":" << std::endl;
        for (auto &i : instLists)
            i->generate(ctx, out);
    }
    void IRTFunction::_generateLV0(Context &ctx, std::ostream &out)
    {
        // precompute:
        std::cout << "function is : " << functionName << std::endl;
        needStackByte = 0;
        int needStackByteSR = 0; // Stack and Register(all in S and R=0)
        for (auto &i : bbList)
            needStackByteSR += i->count_need_stack_byte(ctx);
        int needStackByteA = 4 + ctx.getBackBitMask().get_reg_count() * 4; // RA and other register must be saved(a0-7)
        needStackByte = needStackByteSR + needStackByteA;
        std::cout << "need stack byte is : " << needStackByte << std::endl;

        // label name:
        ctx.create_scope(needStackByte);
        out << "\t.text" << std::endl;
        out << "\t.globl\t" << functionName << std::endl;
        out << functionName << ":" << std::endl;
        // prologue:
        out << "\taddi\tsp,sp,-" << needStackByte << std::endl;
        // save ra
        Address raAddr = ctx.allocate_address(1);
        ctx.insert_symbol("reg_ra", raAddr);
        out << "\tsw\tra," << raAddr.get_offset() << "(sp)" << std::endl;
        // insert other register into symbol table
        for (int i = 0; i < RV32RegUtil::reg_count; i++)
            if (ctx.getBackBitMask().is_true(i))
            {
                Address tmpAddr = ctx.allocate_address(1);
                ctx.insert_symbol("reg_" + RV32RegUtil::get_x_name(i), tmpAddr);
            }
        for (auto &i : bbList)
            i->generate(ctx, out);
        ctx.end_scope();
    }
    void IRTRoot::_generateLV0(Context &ctx, std::ostream &out)
    {
        std::cout << "generate begin" << std::endl;
        out << "\t.data" << std::endl;
        for (auto &i : instList)
        {
            i->generate(ctx, out);
        }
        for (auto &i : funcList)
        {
            i->generate(ctx, out);
        }
    }

}