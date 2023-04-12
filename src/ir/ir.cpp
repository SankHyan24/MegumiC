#include <ir/ir.hpp>
#include <string>
namespace
{
    // mapping from BinOp:
    char binOpStr[20][5] = {
        "+",
        "-",
        "*",
        "/",
        "%",
        "<=",
        ">=",
        "<",
        ">",
        "==",
        "!=",
        "&&",
        "||"};
}

namespace MC::IR
{
    std::ostream &operator<<(std::ostream &os, const BinOp &p)
    {
        os << binOpStr[int(p)];
        return os;
    }
}