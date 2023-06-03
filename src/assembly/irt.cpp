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

}