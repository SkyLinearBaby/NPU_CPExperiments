#include "CmpInst.h"
#include "../Types/IntegerType.h"

namespace IR {

    CmpInst::CmpInst(const std::string & op, std::shared_ptr<Value> lhs, std::shared_ptr<Value> rhs)
        : Instruction(nullptr, IRInstOperator::IRINST_OP_ICMP_EQ, IntegerType::getTypeBool())
    {
        pred = getPredicateFromString(op);
        addOperand(lhs.get());
        addOperand(rhs.get());
    }

    CmpInst::Predicate CmpInst::getPredicateFromString(const std::string & op)
    {
        if (op == "==")
            return Predicate::EQ;
        if (op == "!=")
            return Predicate::NE;
        if (op == "<")
            return Predicate::LT;
        if (op == "<=")
            return Predicate::LE;
        if (op == ">")
            return Predicate::GT;
        if (op == ">=")
            return Predicate::GE;
        return Predicate::EQ; // 默认返回等于
    }

    void CmpInst::toString(std::string & str)
    {
        Value * lhs = getOperand(0);
        Value * rhs = getOperand(1);

        std::string predStr;
        switch (pred) {
            case Predicate::EQ:
                predStr = "eq";
                break;
            case Predicate::NE:
                predStr = "ne";
                break;
            case Predicate::LT:
                predStr = "lt";
                break;
            case Predicate::LE:
                predStr = "le";
                break;
            case Predicate::GT:
                predStr = "gt";
                break;
            case Predicate::GE:
                predStr = "ge";
                break;
        }

        str = getIRName() + " = icmp " + predStr + " " + lhs->getIRName() + ", " + rhs->getIRName();
    }

} // namespace IR