///
/// @file GotoInstruction.cpp
/// @brief 跳转指令即goto指令
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
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>增加条件跳转支持
/// </table>
///

#include "VoidType.h"
#include "GotoInstruction.h"

///
/// @brief 无条件跳转指令的构造函数
/// @param _func 所属函数
/// @param _target 跳转目标
///
GotoInstruction::GotoInstruction(Function * _func, LabelInstruction * _target)
    : Instruction(_func, IRInstOperator::IRINST_OP_GOTO, VoidType::getType()),
      target(_target),
      cond(nullptr),
      true_target(nullptr),
      false_target(nullptr),
      is_conditional(false)
{
}

///
/// @brief 条件跳转指令的构造函数
/// @param _func 所属函数
/// @param _cond 条件值
/// @param _true_target 条件为真时的跳转目标
/// @param _false_target 条件为假时的跳转目标
///
GotoInstruction::GotoInstruction(Function * _func, Value * _cond, LabelInstruction * _true_target, LabelInstruction * _false_target)
    : Instruction(_func, IRInstOperator::IRINST_OP_GOTO, VoidType::getType()),
      target(nullptr),
      cond(_cond),
      true_target(_true_target),
      false_target(_false_target),
      is_conditional(true)
{
}

/// @brief 转换成IR指令文本
void GotoInstruction::toString(std::string & str)
{
    if (is_conditional) {
        str = "bc " + cond->getIRName() + ", label " + true_target->getIRName() + ", label " + false_target->getIRName();
    } else {
        str = "br label " + target->getIRName();
    }
}

///
/// @brief 获取目标Label指令
/// @return LabelInstruction* label指令
///
LabelInstruction * GotoInstruction::getTarget() const
{
    return target;
}

///
/// @brief 是否为条件跳转
/// @return true: 条件跳转, false: 无条件跳转
///
bool GotoInstruction::isConditional() const
{
    return is_conditional;
}
