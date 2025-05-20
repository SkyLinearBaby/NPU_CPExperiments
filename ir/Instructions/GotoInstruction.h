///
/// @file GotoInstruction.h
/// @brief 无条件跳转指令即goto指令
///
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-09-29
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#pragma once

#include <string>

#include "Instruction.h"
#include "LabelInstruction.h"
#include "Function.h"
#include "Value.h"

///
/// @brief 跳转指令
///
class GotoInstruction final : public Instruction {

public:
    ///
    /// @brief 无条件跳转指令的构造函数
    /// @param _func 所属函数
    /// @param _target 跳转目标
    ///
    GotoInstruction(Function * _func, LabelInstruction * _target);

    ///
    /// @brief 条件跳转指令的构造函数
    /// @param _func 所属函数
    /// @param _cond 条件值
    /// @param _true_target 条件为真时的跳转目标
    /// @param _false_target 条件为假时的跳转目标
    ///
    GotoInstruction(Function * _func, Value * _cond, LabelInstruction * _true_target, LabelInstruction * _false_target);

    /// @brief 转换成字符串
    void toString(std::string & str) override;

    ///
    /// @brief 获取目标Label指令
    /// @return LabelInstruction*
    ///
    [[nodiscard]] LabelInstruction * getTarget() const;

    ///
    /// @brief 是否为条件跳转
    /// @return true: 条件跳转, false: 无条件跳转
    ///
    [[nodiscard]] bool isConditional() const;

private:
    ///
    /// @brief 跳转到的目标Label指令
    ///
    LabelInstruction * target;

    ///
    /// @brief 条件值（用于条件跳转）
    ///
    Value * cond;

    ///
    /// @brief 条件为真时的跳转目标
    ///
    LabelInstruction * true_target;

    ///
    /// @brief 条件为假时的跳转目标
    ///
    LabelInstruction * false_target;

    ///
    /// @brief 是否为条件跳转
    ///
    bool is_conditional;
};
