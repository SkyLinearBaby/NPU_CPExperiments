#pragma once

#include "../Instruction.h"
#include <memory>
#include <string>

namespace IR {

    class CmpInst : public Instruction {
    public:
        enum class Predicate {
            EQ, // equal
            NE, // not equal
            LT, // less than
            LE, // less than or equal
            GT, // greater than
            GE  // greater than or equal
        };

        CmpInst(const std::string & op, std::shared_ptr<Value> lhs, std::shared_ptr<Value> rhs);

        static Predicate getPredicateFromString(const std::string & op);

        // 获取比较谓词
        Predicate getPredicate() const
        {
            return pred;
        }

        // 获取操作数
        std::shared_ptr<Value> getLeftOperand()
        {
            return std::shared_ptr<Value>(getOperand(0));
        }
        std::shared_ptr<Value> getRightOperand()
        {
            return std::shared_ptr<Value>(getOperand(1));
        }

        // LLVM IR 格式输出
        void toString(std::string & str);

    private:
        Predicate pred;
    };

} // namespace IR