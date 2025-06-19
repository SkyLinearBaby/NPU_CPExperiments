///
/// @file MemAccessInstruction.h
/// @brief 内存访问指令
///
#pragma once

#include "Instruction.h"
#include "Types/VoidType.h"

///
/// @brief 内存加载指令，对应 %result = *%ptr
///
class LoadInstruction : public Instruction {
public:
    ///
    /// @brief 构造函数
    /// @param func 所属函数
    /// @param ptr 指针
    /// @param type 结果类型
    ///
    LoadInstruction(Function * func, Value * ptr, Type * type) : Instruction(func, IRInstOperator::IRINST_OP_LOAD, type)
    {
        addOperand(ptr);
    }

    ///
    /// @brief 转换为字符串
    /// @param str 输出字符串
    ///
    void toString(std::string & str) override
    {
        str = getIRName() + " = *" + getOperand(0)->getIRName();
    }
};

///
/// @brief 内存存储指令，对应 *%ptr = %value
///
class StoreInstruction : public Instruction {
public:
    ///
    /// @brief 构造函数
    /// @param func 所属函数
    /// @param ptr 指针
    /// @param value 值
    ///
    StoreInstruction(Function * func, Value * ptr, Value * value)
        : Instruction(func, IRInstOperator::IRINST_OP_STORE, VoidType::getType())
    {
        // 安全检查
        if (!ptr) {
           // minic_log(LOG_ERROR, "存储指令的指针为空");
            return;
        }

        if (!value) {
          //  minic_log(LOG_ERROR, "存储指令的值为空");
            return;
        }

        // 检查指针类型
        Type * ptrType = ptr->getType();
        if (!ptrType) {
         //   minic_log(LOG_ERROR, "存储指令的指针类型为空");
            return;
        }

        // 检查值类型
        Type * valueType = value->getType();
        if (!valueType) {
         //   minic_log(LOG_ERROR, "存储指令的值类型为空");
            return;
        }

      /*  minic_log(LOG_DEBUG,
                  "创建存储指令: 指针类型ID=%d, 值类型ID=%d",
                  static_cast<int>(ptrType->getTypeID()),
                  static_cast<int>(valueType->getTypeID()));*/

        addOperand(ptr);
        addOperand(value);
    }

    ///
    /// @brief 转换为字符串
    /// @param str 输出字符串
    ///
    void toString(std::string & str) override
    {
        if (getOperands().size() < 2) {
            str = "store <无效操作数>";
            return;
        }

        str = "*" + getOperand(0)->getIRName() + " = " + getOperand(1)->getIRName();
    }
};