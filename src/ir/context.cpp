#include <ir/context.hpp>

namespace MC::IR
{
    VarInfo::VarInfo(std::string name, bool is_array, std::vector<int> shape)
        : shape(shape), is_array(is_array), name(name) {}

    Context::Context()
    {
        // start1
    }
}