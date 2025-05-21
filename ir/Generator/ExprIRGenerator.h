#pragma once

#include "../IRCode.h"
#include "../Value.h"
#include <string>
#include <memory>

namespace IR {

    class ExprIRGenerator {
    public:
        // 表示基本块的标签
        struct Label {
            std::string name;
            explicit Label(const std::string & name) : name(name)
            {}
        };

        // 表达式求值的结果
        struct ExprResult {
            std::shared_ptr<Value> value; // 表达式的值
            std::string trueLabel;        // 条件为真时跳转的标签
            std::string falseLabel;       // 条件为假时跳转的标签
        };

        ExprIRGenerator(Function * func, InterCode & irCode);

        // 生成关系运算符的IR
        ExprResult generateRelationalExpr(const std::string & op,
                                          std::shared_ptr<Value> left,
                                          std::shared_ptr<Value> right,
                                          const std::string & trueLabel = "",
                                          const std::string & falseLabel = "");

        // 生成逻辑与的IR
        ExprResult generateLogicalAnd(std::shared_ptr<Value> left,
                                      std::shared_ptr<Value> right,
                                      const std::string & trueLabel,
                                      const std::string & falseLabel);

        // 生成逻辑或的IR
        ExprResult generateLogicalOr(std::shared_ptr<Value> left,
                                     std::shared_ptr<Value> right,
                                     const std::string & trueLabel,
                                     const std::string & falseLabel);

        // 生成逻辑非的IR
        ExprResult generateLogicalNot(std::shared_ptr<Value> operand,
                                      const std::string & trueLabel,
                                      const std::string & falseLabel);

        // 生成条件跳转指令
        void generateCondBranch(std::shared_ptr<Value> condition,
                                const std::string & trueLabel,
                                const std::string & falseLabel);

        // 生成无条件跳转指令
        void generateBranch(const std::string & label);

        // 创建新的基本块
        std::string createBasicBlock(const std::string & prefix = "");

        // 将布尔值(i1)转换为整型(i32)
        std::shared_ptr<Value> boolToInt(std::shared_ptr<Value> boolValue);

        // 将整型转换为布尔值
        std::shared_ptr<Value> intToBool(std::shared_ptr<Value> intValue);

    private:
        Function * func;
        InterCode & irCode;
        int labelCounter = 0;

        // 生成唯一的标签名
        std::string generateUniqueLabel(const std::string & prefix);
    };

} // namespace IR