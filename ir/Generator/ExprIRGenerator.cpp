#include "ExprIRGenerator.h"
#include "../Instructions/CmpInst.h"
#include "../Instructions/BranchInst.h"
#include "../Instructions/ZExtInst.h"
#include "../Instructions/TruncInst.h"
#include "../Types/IntegerType.h"

namespace IR {

    ExprIRGenerator::ExprIRGenerator(IRCode & irCode) : irCode(irCode)
    {}

    std::string ExprIRGenerator::generateUniqueLabel(const std::string & prefix)
    {
        return prefix + std::to_string(++labelCounter);
    }

    std::string ExprIRGenerator::createBasicBlock(const std::string & prefix)
    {
        std::string label = generateUniqueLabel(prefix.empty() ? "block" : prefix);
        irCode.createBasicBlock(label);
        return label;
    }

    ExprResult ExprIRGenerator::generateRelationalExpr(const std::string & op,
                                                       std::shared_ptr<Value> left,
                                                       std::shared_ptr<Value> right,
                                                       const std::string & trueLabel,
                                                       const std::string & falseLabel)
    {
        // 创建比较指令
        auto cmpInst = std::make_shared<CmpInst>(op, left, right);
        irCode.addInstruction(cmpInst);

        ExprResult result;
        result.value = cmpInst;
        result.trueLabel = trueLabel;
        result.falseLabel = falseLabel;

        // 如果提供了跳转标签，生成条件跳转
        if (!trueLabel.empty() && !falseLabel.empty()) {
            generateCondBranch(cmpInst, trueLabel, falseLabel);
        }

        return result;
    }

    ExprResult ExprIRGenerator::generateLogicalAnd(std::shared_ptr<Value> left,
                                                   std::shared_ptr<Value> right,
                                                   const std::string & trueLabel,
                                                   const std::string & falseLabel)
    {
        // 创建中间标签
        std::string midLabel = createBasicBlock("and.right");

        // 生成左操作数的条件判断
        auto leftResult = generateRelationalExpr("", left, nullptr, midLabel, falseLabel);

        // 生成右操作数的基本块
        irCode.setCurrentBasicBlock(midLabel);

        // 生成右操作数的条件判断
        auto rightResult = generateRelationalExpr("", right, nullptr, trueLabel, falseLabel);

        ExprResult result;
        result.value = rightResult.value;
        result.trueLabel = trueLabel;
        result.falseLabel = falseLabel;

        return result;
    }

    ExprResult ExprIRGenerator::generateLogicalOr(std::shared_ptr<Value> left,
                                                  std::shared_ptr<Value> right,
                                                  const std::string & trueLabel,
                                                  const std::string & falseLabel)
    {
        // 创建中间标签
        std::string midLabel = createBasicBlock("or.right");

        // 生成左操作数的条件判断
        auto leftResult = generateRelationalExpr("", left, nullptr, trueLabel, midLabel);

        // 生成右操作数的基本块
        irCode.setCurrentBasicBlock(midLabel);

        // 生成右操作数的条件判断
        auto rightResult = generateRelationalExpr("", right, nullptr, trueLabel, falseLabel);

        ExprResult result;
        result.value = rightResult.value;
        result.trueLabel = trueLabel;
        result.falseLabel = falseLabel;

        return result;
    }

    ExprResult ExprIRGenerator::generateLogicalNot(std::shared_ptr<Value> operand,
                                                   const std::string & trueLabel,
                                                   const std::string & falseLabel)
    {
        // 对于逻辑非，我们只需要交换true和false标签
        auto result = generateRelationalExpr("", operand, nullptr, falseLabel, trueLabel);

        // 注意：这里我们交换了结果中的标签
        result.trueLabel = trueLabel;
        result.falseLabel = falseLabel;

        return result;
    }

    void ExprIRGenerator::generateCondBranch(std::shared_ptr<Value> condition,
                                             const std::string & trueLabel,
                                             const std::string & falseLabel)
    {
        auto brInst = std::make_shared<BranchInst>(condition, trueLabel, falseLabel);
        irCode.addInstruction(brInst);
    }

    void ExprIRGenerator::generateBranch(const std::string & label)
    {
        auto brInst = std::make_shared<BranchInst>(label);
        irCode.addInstruction(brInst);
    }

    std::shared_ptr<Value> ExprIRGenerator::boolToInt(std::shared_ptr<Value> boolValue)
    {
        auto i32Type = std::make_shared<IntegerType>(32);
        auto zextInst = std::make_shared<ZExtInst>(boolValue, i32Type);
        irCode.addInstruction(zextInst);
        return zextInst;
    }

    std::shared_ptr<Value> ExprIRGenerator::intToBool(std::shared_ptr<Value> intValue)
    {
        auto i1Type = std::make_shared<IntegerType>(1);
        auto truncInst = std::make_shared<TruncInst>(intValue, i1Type);
        irCode.addInstruction(truncInst);
        return truncInst;
    }

} // namespace IR