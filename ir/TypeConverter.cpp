///
/// @file TypeConverter.cpp
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
#include "TypeConverter.h"
#include "Types/IntegerType.h"
#include "Values/ConstInt.h"

Value* TypeConverter::convertIntToBool(Value* val, Function* func) {
    if (val->getType()->isInt1Byte()) {
        return val;  // 已经是布尔型，直接返回
    }
    
    // 创建比较指令：val != 0
    auto zero = new ConstInt(0);
    auto cmp = new BinaryInstruction(func, IRInstOperator::IRINST_OP_SUB_I, val, zero, IntegerType::getTypeBool());
    return cmp;
}

Value* TypeConverter::convertBoolToInt(Value* val, Function* func) {
    if (val->getType()->isInt32Type()) {
        return val;  // 已经是整型，直接返回
    }
    
    // 创建零扩展指令
    auto zext = new ZExtInst(func, val, IntegerType::getTypeInt());
    return zext;
}

Value* TypeConverter::handleLessThanComparison(Value* left, Value* right, Function* func, Type* targetType) {
    // 确保操作数类型一致
    if (left->getType()->isInt1Byte() != right->getType()->isInt1Byte()) {
        // 如果一个是布尔型，另一个是整型，将布尔型转换为整型
        if (left->getType()->isInt1Byte()) {
            left = convertBoolToInt(left, func);
        }
        if (right->getType()->isInt1Byte()) {
            right = convertBoolToInt(right, func);
        }
    }
    
    // 创建小于比较指令
    auto cmp = new BinaryInstruction(func, IRInstOperator::IRINST_OP_SUB_I, left, right, IntegerType::getTypeBool());
    
    // 如果结果需要整型，进行零扩展
    if (targetType->isInt32Type()) {
        return convertBoolToInt(cmp, func);
    }
    
    return cmp;
}

bool TypeConverter::isIntType(Type* type) {
    return type->isIntegerType() || type->isInt1Byte() || type->isInt32Type();
} 