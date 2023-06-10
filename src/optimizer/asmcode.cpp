#include <optimizer/asmcode.hpp>
namespace
{
    using namespace MC::OPT;
    AsmOp fromStr2AsmOp(std::string str)
    {
        if (str == "call")
            return AsmOp::CALL;
        if (str == "lw")
            return AsmOp::L_S;
        if (str == "sw")
            return AsmOp::L_S;
        if (str == "lui")
            return AsmOp::LUI;
        if (str == "bne")
            return AsmOp::BNE;
        if (str == "jmp" || str == "j")
            return AsmOp::JMP;
        if (str == "jr")
            return AsmOp::JR;
        if (str == "la")
            return AsmOp::LA;
        if (str == "li")
            return AsmOp::LI;
        if (str == "addi" || str == "add" || str == "sub" || str == "mul" || str == "div" || str == "rem" || str == "slt" || str == "and" || str == "andi" || str == "sgt" || str == "xor" || str == "xori" || str == "seqz" || str == "snez" || str == "or" || str == "ori")
            return AsmOp::BINOP;
        throw std::runtime_error("fromStr2AsmOp: " + str + " is not a valid asm op");
    }
}

namespace MC::OPT
{
    AsmCode::AsmCode(std::string target_inst_str)
    {
        // parse the target_inst_str
        // 1. delete the " " or "\t" in the head
        std::string target_inst_str_trimed = target_inst_str;
        while (target_inst_str_trimed[0] == ' ' || target_inst_str_trimed[0] == '\t')
            target_inst_str_trimed.erase(0, 1);
        // 2. delete the " " or "\t" in the tail
        while (target_inst_str_trimed[target_inst_str_trimed.size() - 1] == ' ' || target_inst_str_trimed[target_inst_str_trimed.size() - 1] == '\t')
            target_inst_str_trimed.erase(target_inst_str_trimed.size() - 1, 1);

        // 3. get the comment
        std::string rt_comment = ""; // after the "//"
        std::string::size_type pos = target_inst_str_trimed.find("//");
        if (pos != std::string::npos)
        {
            rt_comment = target_inst_str_trimed.substr(pos + 2, target_inst_str_trimed.size() - pos - 2);
            target_inst_str_trimed.erase(pos, target_inst_str_trimed.size() - pos);
        }
        // if only have comment
        if (target_inst_str_trimed.size() == 0)
        {
            this->comment = rt_comment;
            this->op = AsmOp::COMMENT;
            return;
        }
        // 4. get the label
        // If have ":", then it is a label
        std::string rt_label = ""; // before the ":"
        pos = target_inst_str_trimed.find(":");
        if (pos != std::string::npos)
        {
            this->op = AsmOp::LABEL;
            rt_label = target_inst_str_trimed.substr(0, pos);
            target_inst_str_trimed.erase(0, pos + 1);
            this->label = rt_label;
            return;
        }
        // 5. get the asmstr
        // if .xxx, then it is a asmstr
        if (target_inst_str_trimed[0] == '.')
        {
            this->op = AsmOp::ASM;
            // if have ' ', split the asmstr
            pos = target_inst_str_trimed.find(" ");
            if (pos != std::string::npos)
            {
                this->asmstr = target_inst_str_trimed.substr(1, pos - 1);
                target_inst_str_trimed.erase(0, pos + 1);
                // get the arg
                this->asmstr_arg = target_inst_str_trimed;
            }
            else
                this->asmstr = target_inst_str_trimed.substr(1, target_inst_str_trimed.size() - 1);
            return;
        }
        // 6. get the op
        // get the optype from the target_inst_str_trimed
        std::string rt_op = "";
        pos = target_inst_str_trimed.find(" ");
        if (pos != std::string::npos)
        {
            rt_op = target_inst_str_trimed.substr(0, pos);
            target_inst_str_trimed.erase(0, pos + 1);
            this->opstr = rt_op;
            this->op = fromStr2AsmOp(rt_op);
            if (this->op == MC::OPT::AsmOp::CALL)
            {
                this->label = target_inst_str_trimed;
                return;
            }
            // get the arg
            std::string rt_arg1 = "";
            std::string rt_arg2 = "";
            std::string rt_arg3 = "";
            pos = target_inst_str_trimed.find(",");
            if (pos != std::string::npos)
            {
                rt_arg1 = target_inst_str_trimed.substr(0, pos);
                target_inst_str_trimed.erase(0, pos + 1);
                pos = target_inst_str_trimed.find(",");
                if (pos != std::string::npos)
                {
                    rt_arg2 = target_inst_str_trimed.substr(0, pos);
                    target_inst_str_trimed.erase(0, pos + 1);
                    rt_arg3 = target_inst_str_trimed;
                }
                else
                {
                    pos = target_inst_str_trimed.find("("); // load/store arg1, arg2(arg3)
                    if (pos != std::string::npos)
                    {
                        rt_arg2 = target_inst_str_trimed.substr(0, pos);
                        target_inst_str_trimed.erase(0, pos + 1);
                        pos = target_inst_str_trimed.find(")");
                        if (pos != std::string::npos)
                        {
                            rt_arg3 = target_inst_str_trimed.substr(0, pos);
                            target_inst_str_trimed.erase(0, pos + 1);
                        }
                        else
                            throw std::runtime_error("AsmCode::AsmCode(std::string target_inst_str): " + target_inst_str + " is not a valid asm code");
                    }
                    else
                        rt_arg2 = target_inst_str_trimed;
                }
            }
            else
                rt_arg1 = target_inst_str_trimed;

            // remove the " " or "\t" in the arg
            while (rt_arg1[0] == ' ' || rt_arg1[0] == '\t')
                rt_arg1.erase(0, 1);
            while (rt_arg1[rt_arg1.size() - 1] == ' ' || rt_arg1[rt_arg1.size() - 1] == '\t')
                rt_arg1.erase(rt_arg1.size() - 1, 1);
            while (rt_arg2[0] == ' ' || rt_arg2[0] == '\t')
                rt_arg2.erase(0, 1);
            while (rt_arg2[rt_arg2.size() - 1] == ' ' || rt_arg2[rt_arg2.size() - 1] == '\t')
                rt_arg2.erase(rt_arg2.size() - 1, 1);
            while (rt_arg3[0] == ' ' || rt_arg3[0] == '\t')
                rt_arg3.erase(0, 1);
            while (rt_arg3[rt_arg3.size() - 1] == ' ' || rt_arg3[rt_arg3.size() - 1] == '\t')
                rt_arg3.erase(rt_arg3.size() - 1, 1);
            this->arg1 = rt_arg1;
            this->arg2 = rt_arg2;
            this->arg3 = rt_arg3;
        }
        else
        {
            rt_op = target_inst_str_trimed;
            target_inst_str_trimed.erase(0, target_inst_str_trimed.size());
            // 事实上我的编译器暂时不会生成这类代码
            throw std::runtime_error("AsmCode::AsmCode(std::string target_inst_str): " + target_inst_str + " is not a valid asm code");
        }
    }

    std::string AsmCode::getString()
    {
        return "";
    }

    void AsmCode::Dump(std::ostream &out)
    {
        switch (this->op)
        {
        case AsmOp::COMMENT:
            // out << "\t//" << this->comment << std::endl;
            break;
        case AsmOp::LABEL:
            out << this->label << ":"
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::ASM:
            out << "\t." << this->asmstr << (this->asmstr_arg.size() == 0 ? "" : " " + this->asmstr_arg)
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::CALL:
            out << "\tcall\t" << this->label
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::L_S:
            out << "\t" << this->opstr << "\t" << this->arg1 << ", " << this->arg2 << "(" << this->arg3 << ")"
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::LUI:
            out << "\tlui\t" << this->arg1 << ", " << this->arg2
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::BNE:
            out << "\tbne\t" << this->arg1 << ", " << this->arg2 << ", " << this->arg3
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::JMP:
            out << "\tj\t" << this->arg1
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::JR:
            out << "\tjr\t" << this->arg1
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::LA:
            out << "\tla\t" << this->arg1 << ", " << this->arg2 << "  "
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::LI:
            out << "\tli\t" << this->arg1 << ", " << this->arg2 << "   "
                << getCommentStr2Inst() << std::endl;
            break;
        case AsmOp::BINOP:
            out << "\t" << this->opstr << "\t" << this->arg1 << ", " << this->arg2 << (this->arg3.size() == 0 ? "" : ", " + this->arg3)
                << getCommentStr2Inst() << std::endl;
            break;
        default:
            out << "// 我还不认识( •̀ ω •́ )y" << std::endl;
            throw std::runtime_error("AsmCode::Dump(std::ostream &out): " + std::to_string((int)this->op) + " is not a valid asm op");
            break;
        }
    }
}