///
/// @file TruncInst.cpp
/// @brief 截断指令（用于整型到布尔型的转换）
///
/// @author wangbo
/// @version 1.0
/// @date 2025-05-21
///
/// @copyright Copyright (c) 2025
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-25 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#include "TruncInst.h"

/// @brief 构造函数
/// @param _func 所属函数
/// @param _srcVal 源操作数（整型）
/// @param _type 目标类型（布尔型）
TruncInst::TruncInst(Function * _func, Value * _srcVal, Type * _type)
    : Instruction(_func, IRInstOperator::IRINST_OP_TRUNC, _type)
{
    addOperand(_srcVal);
}

/// @brief 转换成字符串
/// @param str 转换后的字符串
void TruncInst::toString(std::string & str)
{
    Value * src = getOperand(0);

    // 截断指令，整型到布尔型的转换
    str = getIRName() + " = trunc " + src->getIRName() + " to bool";
}