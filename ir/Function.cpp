﻿///
/// @file Function.cpp
/// @brief 函数实现
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-11-21
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// </table>
///

#include <cstdlib>
#include <string>
#include <iostream>

#include "IRConstant.h"
#include "Function.h"
#include "Types/ArrayType.h"
#include "Types/PointerType.h"

/// @brief 指定函数名字、函数类型的构造函数
/// @param _name 函数名称
/// @param _type 函数类型
/// @param _builtin 是否是内置函数
Function::Function(std::string _name, FunctionType * _type, bool _builtin)
    : GlobalValue(_type, _name), builtIn(_builtin)
{
    returnType = _type->getReturnType();

    // 设置对齐大小
    setAlignment(1);
}

///
/// @brief 析构函数
/// @brief 释放函数占用的内存和IR指令代码
/// @brief 注意：IR指令代码并未释放，需要手动释放
Function::~Function()
{
    Delete();
}

/// @brief 获取函数返回类型
/// @return 返回类型
Type * Function::getReturnType()
{
    return returnType;
}

/// @brief 获取函数的形参列表
/// @return 形参列表
std::vector<FormalParam *> & Function::getParams()
{
    return params;
}

/// @brief 获取函数内的IR指令代码
/// @return IR指令代码
InterCode & Function::getInterCode()
{
    return code;
}

/// @brief 判断该函数是否是内置函数
/// @return true: 内置函数，false：用户自定义
bool Function::isBuiltin()
{
    return builtIn;
}

/// @brief 函数指令信息输出
/// @param str 函数指令
void Function::toString(std::string & str)
{
    if (builtIn) {
        // 内置函数不输出任何内容，让IRCompiler自己处理
        return;
    }

    // 输出函数头
    str = "define " + getReturnType()->toString() + " " + getIRName() + "(";

    bool firstParam = false;
    for (auto & param: params) {

        if (!firstParam) {
            firstParam = true;
        } else {
            str += ", ";
        }

        std::string type_str;
        std::string name_str = param->getIRName();
        if (param->getType()->isArrayType()) {
            // 只输出最内层类型
            const ArrayType * arrType = static_cast<const ArrayType *>(param->getType());
            const Type * baseType = arrType->getBaseElementType();
            while (baseType && baseType->isArrayType()) {
                baseType = static_cast<const ArrayType *>(baseType)->getBaseElementType();
            }
            type_str = baseType ? baseType->toString() : "void";
            // 不拼接任何维度
        } else {
            type_str = param->getType()->toString();
        }
        std::string param_str = type_str + " " + name_str;

        // 调试输出参数类型和维度
        std::cout << "[DEBUG] param IRName: " << param->getIRName() << ", type: " << param->getType()->toString()
                  << ", isArrayType: " << param->getType()->isArrayType() << std::endl;
        if (param->getType()->isArrayType()) {
            const ArrayType * arrType = static_cast<const ArrayType *>(param->getType());
            std::vector<uint32_t> dims = arrType->getDimensions();
            std::cout << "[DEBUG] dims: ";
            for (uint32_t dim: dims) {
                std::cout << dim << " ";
            }
            std::cout << std::endl;
            for (uint32_t dim: dims) {
                param_str += "[" + std::to_string(dim) + "]";
            }
        }

        str += param_str;
    }

    str += ")\n";

    str += "{\n";

    // 输出局部变量的名字与IR名字
    for (auto & var: this->varsVector) {

        // 修复数组类型变量的declare格式
        std::string typeStr = var->getType()->toString();
        std::string varNameWithDims = var->getIRName();
        if (var->getType()->isArrayType()) {
            const ArrayType * arrType = static_cast<const ArrayType *>(var->getType());
            const Type * baseType = arrType->getBaseElementType();
            typeStr = baseType ? baseType->toString() : "void";
            std::vector<uint32_t> dims = arrType->getDimensions();
            for (uint32_t dim: dims) {
                varNameWithDims += "[" + std::to_string(dim) + "]";
            }
        }
        str += "\tdeclare " + typeStr + " " + varNameWithDims;

        std::string realName = var->getName();
        if (!realName.empty()) {
            str += " ; " + std::to_string(var->getScopeLevel()) + ":" + realName;
        }

        str += "\n";
    }

    // 输出临时变量的declare形式
    // 遍历所有的线性IR指令，文本输出
    for (auto & inst: code.getInsts()) {

        if (inst->hasResultValue()) {
            // 修复临时变量declare格式
            std::string typeStr = inst->getType()->toString();
            std::string varNameWithDims = inst->getIRName();
            if (inst->getType()->isArrayType()) {
                const ArrayType * arrType = static_cast<const ArrayType *>(inst->getType());
                const Type * baseType = arrType->getBaseElementType();
                typeStr = baseType ? baseType->toString() : "void";
                // 移除维度信息拼接，保持变量名简洁
            } else if (inst->getType()->isPointerType()) {
                // 处理指针类型，确保不包含维度信息
                const PointerType * ptrType = static_cast<const PointerType *>(inst->getType());
                const Type * pointeeType = ptrType->getPointeeType();
                if (pointeeType->isArrayType()) {
                    // 如果指向数组类型，只显示基本类型
                    const ArrayType * arrType = static_cast<const ArrayType *>(pointeeType);
                    const Type * baseType = arrType->getBaseElementType();
                    typeStr = (baseType ? baseType->toString() : "void") + "*";
                } else {
                    typeStr = pointeeType->toString() + "*";
                }
            }
            str += "\tdeclare " + typeStr + " " + varNameWithDims + "\n";
        }
    }

    // 遍历所有的线性IR指令，文本输出
    for (auto & inst: code.getInsts()) {

        std::string instStr;
        inst->toString(instStr);

        if (!instStr.empty()) {

            // Label指令不加Tab键
            if (inst->getOp() == IRInstOperator::IRINST_OP_LABEL) {
                str += instStr + "\n";
            } else {
                str += "\t" + instStr + "\n";
            }
        }
    }

    // 输出函数尾部
    str += "}\n";
}

/// @brief 设置函数出口指令
/// @param inst 出口Label指令
void Function::setExitLabel(Instruction * inst)
{
    exitLabel = inst;
}

/// @brief 获取函数出口指令
/// @return 出口Label指令
LabelInstruction * Function::getExitLabel()
{
    return static_cast<LabelInstruction *>(exitLabel);
}

/// @brief 设置函数返回值变量
/// @param val 返回值变量，要求必须是局部变量，不能是临时变量
void Function::setReturnValue(LocalVariable * val)
{
    returnValue = val;
}

/// @brief 获取函数返回值变量
/// @return 返回值变量
LocalVariable * Function::getReturnValue()
{
    return returnValue;
}

/// @brief 获取最大栈帧深度
/// @return 栈帧深度
int Function::getMaxDep()
{
    return maxDepth;
}

/// @brief 设置最大栈帧深度
/// @param dep 栈帧深度
void Function::setMaxDep(int dep)
{
    maxDepth = dep;

    // 设置函数栈帧被重定位标记，用于生成不同的栈帧保护代码
    relocated = true;
}

/// @brief 获取本函数需要保护的寄存器
/// @return 要保护的寄存器
std::vector<int32_t> & Function::getProtectedReg()
{
    return protectedRegs;
}

/// @brief 获取本函数需要保护的寄存器字符串
/// @return 要保护的寄存器
std::string & Function::getProtectedRegStr()
{
    return protectedRegStr;
}

/// @brief 获取函数调用参数个数的最大值
/// @return 函数调用参数个数的最大值
int Function::getMaxFuncCallArgCnt()
{
    return maxFuncCallArgCnt;
}

/// @brief 设置函数调用参数个数的最大值
/// @param count 函数调用参数个数的最大值
void Function::setMaxFuncCallArgCnt(int count)
{
    maxFuncCallArgCnt = count;
}

/// @brief 函数内是否存在函数调用
/// @return 是否存在函调用
bool Function::getExistFuncCall()
{
    return funcCallExist;
}

/// @brief 设置函数是否存在函数调用
/// @param exist true: 存在 false: 不存在
void Function::setExistFuncCall(bool exist)
{
    funcCallExist = exist;
}

/// @brief 新建变量型Value。先检查是否存在，不存在则创建，否则失败
/// @param name 变量ID
/// @param type 变量类型
/// @param scope_level 局部变量的作用域层级
LocalVariable * Function::newLocalVarValue(Type * type, std::string name, int32_t scope_level)
{
    // 创建局部变量
    LocalVariable * var = new LocalVariable(type, name, scope_level);
    if (!var) {
        return nullptr;
    }

    // 添加到函数的局部变量列表中
    varsVector.push_back(var);

    return var;
}

/// @brief 新建一个内存型的Value，并加入到符号表，用于后续释放空间
/// \param type 变量类型
/// \return 临时变量Value
MemVariable * Function::newMemVariable(Type * type)
{
    // 肯定唯一存在，直接插入即可
    MemVariable * memValue = new MemVariable(type);

    memVector.push_back(memValue);

    return memValue;
}

/// @brief 清理函数内申请的资源
void Function::Delete()
{
    // 清理IR指令
    code.Delete();

    // 清理Value
    for (auto & var: varsVector) {
        delete var;
    }

    varsVector.clear();
}

///
/// @brief 函数内的Value重命名
///
void Function::renameIR()
{
    // 内置函数忽略
    if (isBuiltin()) {
        return;
    }

    int32_t nameIndex = 0;

    // 形式参数重命名
    for (auto & param: this->params) {
        param->setIRName(IR_TEMP_VARNAME_PREFIX + std::to_string(nameIndex++));
    }

    // 局部变量重命名
    for (auto & var: this->varsVector) {

        var->setIRName(IR_LOCAL_VARNAME_PREFIX + std::to_string(nameIndex++));
    }

    // 遍历所有的指令进行命名
    for (auto inst: this->getInterCode().getInsts()) {
        if (inst->getOp() == IRInstOperator::IRINST_OP_LABEL) {
            inst->setIRName(IR_LABEL_PREFIX + std::to_string(nameIndex++));
        } else if (inst->hasResultValue()) {
            inst->setIRName(IR_TEMP_VARNAME_PREFIX + std::to_string(nameIndex++));
        }
    }
}

///
/// @brief 获取统计的ARG指令的个数
/// @return int32_t 个数
///
int32_t Function::getRealArgcount()
{
    return this->realArgCount;
}

///
/// @brief 用于统计ARG指令个数的自增函数，个数加1
///
void Function::realArgCountInc()
{
    this->realArgCount++;
}

///
/// @brief 用于统计ARG指令个数的清零
///
void Function::realArgCountReset()
{
    this->realArgCount = 0;
}
