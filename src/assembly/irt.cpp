#include <assembly/asm.hpp>
#include <assembly/context.hpp>
#include <assembly/irt.hpp>

namespace MC::ASM
{
    void IRTree::generate(Context &ctx, std::ostream &out)
    {
        this->_generate(ctx, out);
    }

    void IRTree::_generate(Context &ctx, std::ostream &out)
    {
        throw std::runtime_error("Not implement the IRTree _generate method");
    }

    void IRTInst::_generate(Context &ctx, std::ostream &out)
    {
    }

}