///
/// @file TypeConverter.h
/// @brief 类型转换工具类
///
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-11-23
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

#include "Function.h"
#include "Value.h"

/// @brief 类型转换工具类
class TypeConverter {
public:
    /// @brief 将整型转换为布尔型（i32 -> i1）
    /// @param val 源值
    /// @param func 所属函数
    /// @return 转换后的值
    static Value * convertIntToBool(Value * val, Function * func);

    /// @brief 将布尔型转换为整型（i1 -> i32）
    /// @param val 源值
    /// @param func 所属函数
    /// @return 转换后的值
    static Value * convertBoolToInt(Value * val, Function * func);

    /// @brief 处理小于比较操作
    /// @param left 左操作数
    /// @param right 右操作数
    /// @param func 所属函数
    /// @param targetType 目标类型
    /// @return 比较结果
    static Value * handleLessThanComparison(Value * left, Value * right, Function * func, Type * targetType);

private:
    /// @brief 检查是否是整数类型
    /// @param type 类型
    /// @return 是否是整数类型
    static bool isIntType(Type * type);
};