///
/// @file ZExtInst.h
/// @brief 零扩展指令
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
/// <tr><td>2024-11-23 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#pragma once

#include "Instruction.h"

///
/// @brief 零扩展指令
///
class ZExtInst : public Instruction {

public:
    /// @brief 构造函数
    /// @param _func 所属函数
    /// @param _srcVal 源操作数
    /// @param _type 目标类型
    ZExtInst(Function * _func, Value * _srcVal, Type * _type);

    /// @brief 转换成字符串
    /// @param str 转换后的字符串
    void toString(std::string & str) override;
};