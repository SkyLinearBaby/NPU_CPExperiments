///
/// @file TruncInst.h
/// @brief 截断指令（用于整型到布尔型的转换）
///
/// @author wangbo
/// @version 1.0
/// @date 2025-05-21
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-25 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#pragma once

#include "../Instruction.h"

///
/// @brief 截断指令，用于整型到布尔型的转换（非0值转换为true/1，0转换为false/0）
///
class TruncInst : public Instruction {

public:
    /// @brief 构造函数
    /// @param _func 所属函数
    /// @param _srcVal 源操作数（整型）
    /// @param _type 目标类型（布尔型）
    TruncInst(Function * _func, Value * _srcVal, Type * _type);

    /// @brief 转换成字符串
    void toString(std::string & str) override;
};