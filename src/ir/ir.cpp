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
    // bool IR::some(std::function<bool(const MC::IR::OpName &)> callback, bool include_dest) const
    // {
    //     return (include_dest && callback(this->dest)) || callback(this->op1) ||
    //            callback(this->op2) || callback(this->op3);
    // }
    // void IR::forEachOp(std::function<void(const MC::IR::OpName &)> callback, bool include_dest) const
    // {
    //     this->some(
    //         [callback](const MC::IR::OpName &op)
    //         {
    //             callback(op);
    //             return false;
    //         },
    //         include_dest);
    // }

    // void IR::print(std::ostream &out, bool verbose) const
    // {
    //     // print current opcode
    //     this->forEachOp(
    //         [&out](const OpName &op) {
    //         });
    // }
}