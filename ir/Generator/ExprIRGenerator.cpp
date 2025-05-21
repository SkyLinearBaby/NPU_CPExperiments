#include "ExprIRGenerator.h"
#include "../Instructions/CmpInst.h"
#include "../Instructions/GotoInstruction.h"
#include "../Instructions/ZExtInst.h"
#include "../Instructions/TruncInst.h"
#include "../Types/IntegerType.h"

namespace IR {

    ExprIRGenerator::ExprIRGenerator(Function * func, InterCode & irCode) : func(func), irCode(irCode)
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

    ExprIRGenerator::ExprResult ExprIRGenerator::generateRelationalExpr(const std::string & op,
                                                                        std::shared_ptr<Value> left,
                                                                        std::shared_ptr<Value> right,
                                                                        const std::string & trueLabel,
                                                                        const std::string & falseLabel)
    {
        // 创建比较指令
        auto cmpInst = std::make_shared<CmpInst>(op, left, right);
        irCode.addInst(cmpInst.get());

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

    ExprIRGenerator::ExprResult ExprIRGenerator::generateLogicalAnd(std::shared_ptr<Value> left,
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

    ExprIRGenerator::ExprResult ExprIRGenerator::generateLogicalOr(std::shared_ptr<Value> left,
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

    ExprIRGenerator::ExprResult ExprIRGenerator::generateLogicalNot(std::shared_ptr<Value> operand,
                                                                    const std::string & trueLabel,
                                                                    const std::string & falseLabel)
    {
        // 对于逻辑非，我们只需要交换true和false标签

        auto result = generateRelationalExpr("", operand, nullptr, trueLabel, falseLabel);

        // 注意：这里我们交换了结果中的标签
        result.trueLabel = falseLabel;
        result.falseLabel = trueLabel;

        return result;
    }

    void ExprIRGenerator::generateCondBranch(std::shared_ptr<Value> condition,
                                             const std::string & trueLabel,
                                             const std::string & falseLabel)
    {
        // 获取或创建 true 标签指令
        auto trueLabelInst = irCode.getOrCreateLabel(trueLabel);

        // 获取或创建 false 标签指令
        auto falseLabelInst = irCode.getOrCreateLabel(falseLabel);

        // 创建条件跳转指令
        auto brInst = std::make_shared<GotoInstruction>(func, condition.get(), trueLabelInst, falseLabelInst);
        irCode.addInst(brInst.get());
    }

    void ExprIRGenerator::generateBranch(const std::string & label)
    {
        // 获取或创建标签指令
        auto labelInst = irCode.getOrCreateLabel(label);

        // 创建无条件跳转指令
        auto brInst = std::make_shared<GotoInstruction>(func, labelInst);
        irCode.addInst(brInst.get());
    }

    std::shared_ptr<Value> ExprIRGenerator::boolToInt(std::shared_ptr<Value> boolValue)
    {
        auto zextInst = std::make_shared<ZExtInst>(func, boolValue.get(), IntegerType::getTypeInt());
        irCode.addInst(zextInst.get());
        return zextInst;
    }

    std::shared_ptr<Value> ExprIRGenerator::intToBool(std::shared_ptr<Value> intValue)
    {
        auto truncInst = std::make_shared<TruncInst>(func, intValue.get(), IntegerType::getTypeBool());
        irCode.addInst(truncInst.get());
        return truncInst;
    }

} // namespace IR