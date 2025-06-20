///
/// @file IRGenerator.cpp
/// @brief AST遍历产生线性IR的源文件
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <set>
#include "MemAccessInstruction.h"
#include "AST.h"
#include "Common.h"
#include "Function.h"
#include "IRCode.h"
#include "IRGenerator.h"
#include "Module.h"
#include "EntryInstruction.h"
#include "LabelInstruction.h"
#include "ExitInstruction.h"
#include "FuncCallInstruction.h"
#include "BinaryInstruction.h"
#include "MoveInstruction.h"
#include "GotoInstruction.h"
#include "Types/ArrayType.h"
#include "Types/PointerType.h"
#include "Instruction.h"

/// @brief 构造函数
/// @param _root AST的根
/// @param _module 符号表
IRGenerator::IRGenerator(ast_node * _root, Module * _module) : root(_root), module(_module)
{
    /* 叶子节点 */
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_LITERAL_UINT] = &IRGenerator::ir_leaf_node_uint;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_VAR_ID] = &IRGenerator::ir_leaf_node_var_id;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_TYPE] = &IRGenerator::ir_leaf_node_type;

    /* 表达式运算， 加减 */
    ast2ir_handlers[ast_operator_type::AST_OP_SUB] = &IRGenerator::ir_sub;
    ast2ir_handlers[ast_operator_type::AST_OP_ADD] = &IRGenerator::ir_add;
    ast2ir_handlers[ast_operator_type::AST_OP_MUL] = &IRGenerator::ir_mul;
    ast2ir_handlers[ast_operator_type::AST_OP_DIV] = &IRGenerator::ir_div;
    ast2ir_handlers[ast_operator_type::AST_OP_MOD] = &IRGenerator::ir_mod;
    ast2ir_handlers[ast_operator_type::AST_OP_LT] = &IRGenerator::ir_lt;
    ast2ir_handlers[ast_operator_type::AST_OP_GT] = &IRGenerator::ir_gt;
    ast2ir_handlers[ast_operator_type::AST_OP_GE] = &IRGenerator::ir_ge;
    ast2ir_handlers[ast_operator_type::AST_OP_LE] = &IRGenerator::ir_le;
    ast2ir_handlers[ast_operator_type::AST_OP_EQ] = &IRGenerator::ir_eq;
    ast2ir_handlers[ast_operator_type::AST_OP_NEQ] = &IRGenerator::ir_ne;
    ast2ir_handlers[ast_operator_type::AST_OP_UNARY_MINUS] = &IRGenerator::ir_unary_minus;
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_AND] = &IRGenerator::ir_logical_and;
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_OR] = &IRGenerator::ir_logical_or;
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_NOT] = &IRGenerator::ir_logical_not;

    /* 语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_ASSIGN] = &IRGenerator::ir_assign;
    ast2ir_handlers[ast_operator_type::AST_OP_RETURN] = &IRGenerator::ir_return;

    /* 函数调用 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_CALL] = &IRGenerator::ir_function_call;

    /* 函数定义 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_DEF] = &IRGenerator::ir_function_define;
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS] = &IRGenerator::ir_function_formal_params;

    /* 变量定义语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_DECL_STMT] = &IRGenerator::ir_declare_statment;
    ast2ir_handlers[ast_operator_type::AST_OP_VAR_DECL] = &IRGenerator::ir_variable_declare;

    /* 语句块 */
    ast2ir_handlers[ast_operator_type::AST_OP_BLOCK] = &IRGenerator::ir_block;

    /* 编译单元 */
    ast2ir_handlers[ast_operator_type::AST_OP_COMPILE_UNIT] = &IRGenerator::ir_compile_unit;

    /* if语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_IF] = &IRGenerator::ir_if;
    ast2ir_handlers[ast_operator_type::AST_OP_IF_ELSE] = &IRGenerator::ir_if_else;

    // 注册while循环相关处理函数
    ast2ir_handlers[ast_operator_type::AST_OP_WHILE] = &IRGenerator::ir_while;
    ast2ir_handlers[ast_operator_type::AST_OP_BREAK] = &IRGenerator::ir_break;
    ast2ir_handlers[ast_operator_type::AST_OP_CONTINUE] = &IRGenerator::ir_continue;
    /* 数组 */
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_DECL] = &IRGenerator::ir_arraytype;
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_ACCESS] = &IRGenerator::ir_arrayaccess;
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_DIMS] = &IRGenerator::ir_arraydimen;
}

/// @brief 遍历抽象语法树产生线性IR，保存到IRCode中
/// @param root 抽象语法树
/// @param IRCode 线性IR
/// @return true: 成功 false: 失败
bool IRGenerator::run()
{
    ast_node * node;

    // 从根节点进行遍历
    node = ir_visit_ast_node(root);

    return node != nullptr;
}

/// @brief 根据AST的节点运算符查找对应的翻译函数并执行翻译动作
/// @param node AST节点
/// @return 成功返回node节点，否则返回nullptr
ast_node * IRGenerator::ir_visit_ast_node(ast_node * node)
{
    // 空节点
    if (nullptr == node) {
        return nullptr;
    }

    bool result;

    std::unordered_map<ast_operator_type, ast2ir_handler_t>::const_iterator pIter;
    pIter = ast2ir_handlers.find(node->node_type);
    if (pIter == ast2ir_handlers.end()) {
        // 没有找到，则说明当前不支持
        result = (this->ir_default)(node);
    } else {
        result = (this->*(pIter->second))(node);
    }

    if (!result) {
        // 语义解析错误，则出错返回
        node = nullptr;
    }

    return node;
}

/// @brief 未知节点类型的节点处理
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_default(ast_node * node)
{
    // 未知的节点
    printf("Unkown node(%d)\n", (int) node->node_type);
    return true;
}

/// @brief 编译单元AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_compile_unit(ast_node * node)
{
    module->setCurrentFunction(nullptr);

    for (auto son: node->sons) {

        // 遍历编译单元，要么是函数定义，要么是语句
        ast_node * son_node = ir_visit_ast_node(son);
        if (!son_node) {
            // TODO 自行追加语义错误处理
            return false;
        }
    }

    return true;
}

/// @brief 函数定义AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_define(ast_node * node)
{
    if (!node) {
        minic_log(LOG_ERROR, "函数定义节点无效");
        return false;
    }

    // 创建一个函数，用于当前函数处理
    if (module->getCurrentFunction()) {
        // 函数中嵌套定义函数，这是不允许的，错误退出
        minic_log(LOG_ERROR, "函数中嵌套定义函数，这是不允许的");
        return false;
    }

    // 函数定义的AST包含四个孩子
    // 第一个孩子：函数返回类型
    // 第二个孩子：函数名字
    // 第三个孩子：形参列表
    // 第四个孩子：函数体即block
    if (node->sons.size() < 4) {
        minic_log(LOG_ERROR, "函数定义节点格式错误");
        return false;
    }

    ast_node * type_node = node->sons[0];
    ast_node * name_node = node->sons[1];
    ast_node * param_node = node->sons[2];
    ast_node * block_node = node->sons[3];

    if (!type_node || !name_node || !param_node || !block_node) {
        minic_log(LOG_ERROR, "函数定义节点子节点无效");
        return false;
    }

    if (!type_node->type) {
        minic_log(LOG_ERROR, "函数返回类型无效");
        return false;
    }

    if (name_node->name.empty()) {
        minic_log(LOG_ERROR, "函数名无效");
        return false;
    }

    // 创建一个新的函数定义
    Function * newFunc = module->newFunction(name_node->name, type_node->type);
    if (!newFunc) {
        // 新定义的函数已经存在，则失败返回
        minic_log(LOG_ERROR, "函数(%s)重复定义", name_node->name.c_str());
        return false;
    }

    // 当前函数设置有效，变更为当前的函数
    module->setCurrentFunction(newFunc);

    // 进入函数的作用域
    module->enterScope();

    // 获取函数的IR代码列表，用于后面追加指令用，注意这里用的是引用传值
    InterCode & irCode = newFunc->getInterCode();

    // 创建并加入Entry入口指令
    EntryInstruction * entryInst = new EntryInstruction(newFunc);
    if (!entryInst) {
        minic_log(LOG_ERROR, "创建入口指令失败");
        return false;
    }
    irCode.addInst(entryInst);

    // 创建出口指令并不加入出口指令，等函数内的指令处理完毕后加入出口指令
    LabelInstruction * exitLabelInst = new LabelInstruction(newFunc);
    if (!exitLabelInst) {
        minic_log(LOG_ERROR, "创建出口标签指令失败");
        return false;
    }

    // 函数出口指令保存到函数信息中，因为在语义分析函数体时return语句需要跳转到函数尾部，需要这个label指令
    newFunc->setExitLabel(exitLabelInst);

    // 处理形参
    bool result = ir_function_formal_params(param_node);
    if (!result) {
        minic_log(LOG_ERROR, "形参处理失败");
        return false;
    }

    // 将形参的指令添加到函数的IR代码中
    irCode.addInst(param_node->blockInsts);

    // 新建一个Value，用于保存函数的返回值，如果没有返回值可不用申请
    LocalVariable * retValue = nullptr;
    if (!type_node->type->isVoidType()) {
        // 保存函数返回值变量到函数信息中，在return语句翻译时需要设置值到这个变量中
        retValue = static_cast<LocalVariable *>(module->newVarValue(type_node->type));
        if (!retValue) {
            minic_log(LOG_ERROR, "创建返回值变量失败");
            return false;
        }
        newFunc->setReturnValue(retValue);

        // 如果是main函数，初始化返回值为0
        if (name_node->name == "main") {
            MoveInstruction * initInst = new MoveInstruction(newFunc, retValue, module->newConstInt(0));
            if (!initInst) {
                minic_log(LOG_ERROR, "创建初始化指令失败");
                return false;
            }
            irCode.addInst(initInst);
        }
    }

    // 函数内已经进入作用域，内部不再需要做变量的作用域管理
    block_node->needScope = false;

    // 遍历block
    result = ir_block(block_node);
    if (!result) {
        minic_log(LOG_ERROR, "函数体处理失败");
        return false;
    }

    // IR指令追加到当前的节点中
    node->blockInsts.addInst(block_node->blockInsts);

    // node节点的指令移动到函数的IR指令列表中
    irCode.addInst(node->blockInsts);

    // 添加函数出口Label指令，主要用于return语句跳转到这里进行函数的退出
    irCode.addInst(exitLabelInst);

    // 函数出口指令
    ExitInstruction * exitInst = new ExitInstruction(newFunc, retValue);
    if (!exitInst) {
        minic_log(LOG_ERROR, "创建出口指令失败");
        return false;
    }
    irCode.addInst(exitInst);

    // 恢复成外部函数
    module->setCurrentFunction(nullptr);

    // 退出函数的作用域
    module->leaveScope();

    return true;
}

/// @brief 形式参数AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_formal_params(ast_node * node)
{
    if (!node) {
        minic_log(LOG_ERROR, "函数形参节点无效");
        return false;
    }

    // 获取当前正在处理的函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        minic_log(LOG_ERROR, "当前函数未定义");
        return false;
    }

    // 遍历形参列表
    for (auto son: node->sons) {
        if (!son) {
            minic_log(LOG_ERROR, "形参节点无效");
            return false;
        }

        // 新AST结构：formal-param下只有一个array-decl或(type+name)
        if (son->sons.size() == 1 && son->sons[0]->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
            ast_node * arrayNode = son->sons[0];
            // arrayNode的子节点依次为：类型、变量名、维度
            if (arrayNode->sons.size() < 3) {
                minic_log(LOG_ERROR, "数组形参节点格式错误：子节点数量(%d)小于3", (int) arrayNode->sons.size());
                return false;
            }
            /* ast_node * typeNode = arrayNode->sons[0]; */
            ast_node * nameNode = arrayNode->sons[1];
            // 维度节点暂不需要用
            std::string paramName = nameNode->name;
            Type * paramType = arrayNode->type;
            // 创建形参对象
            FormalParam * formalParam = new FormalParam(paramType, paramName);
            if (!formalParam) {
                minic_log(LOG_ERROR, "创建数组形参对象失败");
                return false;
            }
            currentFunc->getParams().push_back(formalParam);
            // 创建形参局部变量
            LocalVariable * paramVar = currentFunc->newLocalVarValue(paramType, paramName);
            if (!paramVar) {
                minic_log(LOG_ERROR, "创建数组形参局部变量失败");
                return false;
            }
            // 添加参数赋值指令
            MoveInstruction * moveInst = new MoveInstruction(currentFunc, paramVar, formalParam);
            if (!moveInst) {
                minic_log(LOG_ERROR, "创建数组参数赋值指令失败");
                return false;
            }
            node->blockInsts.addInst(moveInst);
        } else if (son->sons.size() >= 2) {
            // 普通参数逻辑
            if (son->sons[1]->name.empty()) {
                minic_log(LOG_ERROR, "形参名为空");
                return false;
            }
            // 获取形参类型
            Type * type = son->sons[0]->type;
            if (!type) {
                minic_log(LOG_ERROR, "形参类型无效");
                return false;
            }
            // 获取形参名
            std::string paramName = son->sons[1]->name;
            // 创建形参对象
            FormalParam * formalParam = new FormalParam(type, paramName);
            if (!formalParam) {
                minic_log(LOG_ERROR, "创建形参对象失败");
                return false;
            }
            // 将形参对象添加到函数的参数列表中
            currentFunc->getParams().push_back(formalParam);
            // 创建形参局部变量
            LocalVariable * paramVar = currentFunc->newLocalVarValue(type, paramName);
            if (!paramVar) {
                minic_log(LOG_ERROR, "创建形参局部变量失败");
                return false;
            }
            // 添加参数赋值指令
            MoveInstruction * moveInst = new MoveInstruction(currentFunc, paramVar, formalParam);
            if (!moveInst) {
                minic_log(LOG_ERROR, "创建参数赋值指令失败");
                return false;
            }
            node->blockInsts.addInst(moveInst);
        } else {
            minic_log(LOG_ERROR, "形参节点格式错误：子节点数量(%d)小于2", (int) son->sons.size());
            return false;
        }
    }

    return true;
}

/// @brief 函数调用AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_call(ast_node * node)
{
    if (!node) {
        minic_log(LOG_ERROR, "函数调用节点无效");
        return false;
    }

    // 获取当前正在处理的函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        minic_log(LOG_ERROR, "当前函数未定义");
        return false;
    }

    // 函数调用的节点包含两个节点：
    // 第一个节点：函数名节点
    // 第二个节点：实参列表节点
    if (node->sons.size() < 2) {
        minic_log(LOG_ERROR, "函数调用节点格式错误：子节点数量(%d)小于2", (int) node->sons.size());
        return false;
    }

    if (!node->sons[0]) {
        minic_log(LOG_ERROR, "函数调用节点格式错误：函数名节点无效");
        return false;
    }

    if (node->sons[0]->name.empty()) {
        minic_log(LOG_ERROR, "函数调用节点格式错误：函数名为空");
        return false;
    }

    std::string funcName = node->sons[0]->name;
    ast_node * paramsNode = node->sons[1];

    if (!paramsNode) {
        minic_log(LOG_ERROR, "函数调用参数节点无效");
        return false;
    }

    // 根据函数名查找函数，看是否存在。若不存在则出错
    // 这里约定函数必须先定义后使用
    auto calledFunction = module->findFunction(funcName);
    if (nullptr == calledFunction) {
        minic_log(LOG_ERROR, "函数(%s)未定义或声明", funcName.c_str());
        return false;
    }

    // 当前函数存在函数调用
    currentFunc->setExistFuncCall(true);

    std::vector<Value *> realParams;

    // 处理实参列表
    if (!paramsNode->sons.empty()) {
        int32_t argsCount = (int32_t) paramsNode->sons.size();

        // 当前函数中调用函数实参个数最大值统计，实际上是统计实参传参需在栈中分配的大小
        if (argsCount > currentFunc->getMaxFuncCallArgCnt()) {
            currentFunc->setMaxFuncCallArgCnt(argsCount);
        }

        // 遍历参数列表，孩子是表达式
        // 这里自左往右计算表达式
        for (auto son: paramsNode->sons) {
            if (!son) {
                minic_log(LOG_ERROR, "实参节点无效");
                return false;
            }

            // 遍历Block的每个语句，进行显示或者运算
            ast_node * temp = ir_visit_ast_node(son);
            if (!temp) {
                minic_log(LOG_ERROR, "实参表达式处理失败");
                return false;
            }

            if (!temp->val) {
                minic_log(LOG_ERROR, "实参值为空");
                return false;
            }

            // 数组类型参数特殊处理
            if (temp->val->getType()->isArrayType()) {
                minic_log(LOG_DEBUG, "处理数组类型参数: %s", temp->val->getType()->toString().c_str());

                // 对于数组参数，直接传递数组的基址，不需要创建临时变量
                // 将指令添加到当前节点的指令列表中
                node->blockInsts.addInst(temp->blockInsts);

                // 将数组基址添加到实参列表中
                realParams.push_back(temp->val);
                continue;
            }

            // 数组访问作为参数的特殊处理
            if (son->node_type == ast_operator_type::AST_OP_ARRAY_ACCESS) {
                minic_log(LOG_DEBUG, "处理数组访问参数");

                // 对于数组访问如 a[i]，需要计算元素地址
                Type * arrayType = temp->val->getType();

                // 计算数组基址，这是形如a[k]中计算元素地址的指令
                // 对于多维数组a[k]作为参数传递，应该传递a[k]的基址，而非a[k][0]的地址

                // 计算当前数组中的元素总数（后续所有维度的乘积）
                int totalElems = 1;
                if (arrayType->isArrayType()) {
                    const ArrayType * arrType = static_cast<const ArrayType *>(arrayType);
                    Type * elemType = arrType->getElementType();
                    while (elemType->isArrayType()) {
                        const ArrayType * subArrayType = static_cast<const ArrayType *>(elemType);
                        totalElems *= subArrayType->getNumElements();
                        elemType = subArrayType->getElementType();
                    }
                }

                // 创建一个乘法指令: %t22=mul index,totalElems
                BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_MUL_I,
                                                                    son->sons[1]->val, // 索引
                                                                    module->newConstInt(totalElems),
                                                                    IntegerType::getTypeInt());

                // 计算字节大小的偏移量: %t24=mul %t22,4
                BinaryInstruction * mulSizeInst = new BinaryInstruction(currentFunc,
                                                                        IRInstOperator::IRINST_OP_MUL_I,
                                                                        mulInst,
                                                                        module->newConstInt(4), // 假设每个元素4字节
                                                                        IntegerType::getTypeInt());

                // 计算最终地址: %t23=add base,offset
                BinaryInstruction * addInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_ADD_I,
                                                                    son->sons[0]->val, // 数组基址
                                                                    mulSizeInst,
                                                                    arrayType);

                // 添加指令
                node->blockInsts.addInst(temp->blockInsts);
                node->blockInsts.addInst(mulInst);
                node->blockInsts.addInst(mulSizeInst);
                node->blockInsts.addInst(addInst);

                // 添加到参数列表
                realParams.push_back(addInst);
                continue;
            }

            // 普通参数处理
            // 创建临时变量来保存实参的值
            LocalVariable * tempVar = currentFunc->newLocalVarValue(temp->val->getType());
            if (!tempVar) {
                minic_log(LOG_ERROR, "创建实参临时变量失败");
                return false;
            }

            // 生成赋值指令，将实参的值赋给临时变量
            MoveInstruction * moveInst = new MoveInstruction(currentFunc, tempVar, temp->val);
            if (!moveInst) {
                minic_log(LOG_ERROR, "创建移动指令失败");
                return false;
            }

            // 将指令添加到当前节点的指令列表中
            node->blockInsts.addInst(temp->blockInsts);
            node->blockInsts.addInst(moveInst);

            // 将临时变量添加到实参列表中
            realParams.push_back(tempVar);
        }
    }

    // 检查实参个数是否与形参个数匹配
    if (realParams.size() != calledFunction->getParams().size()) {
        minic_log(LOG_ERROR,
                  "函数(%s)参数个数不匹配，期望%d个，实际%d个",
                  funcName.c_str(),
                  (int) calledFunction->getParams().size(),
                  (int) realParams.size());
        return false;
    }

    // 检查实参类型是否与形参类型匹配
    for (size_t i = 0; i < realParams.size(); i++) {
        Type * paramType = realParams[i]->getType();
        Type * formalType = calledFunction->getParams()[i]->getType();

        if (!paramType || !formalType) {
            minic_log(LOG_ERROR, "函数(%s)第%d个参数类型无效", funcName.c_str(), (int) i + 1);
            return false;
        }

        // 数组参数特殊处理
        if (formalType->isArrayType() && paramType->isArrayType()) {
            // 对于数组参数，第一维可以不同，但其他维度必须相同
            const ArrayType * formalArrayType = static_cast<const ArrayType *>(formalType);
            const ArrayType * paramArrayType = static_cast<const ArrayType *>(paramType);

            // 如果形参第一维是0，则可以接受任意第一维的实参
            if (formalArrayType->getNumElements() == 0) {
                // 验证元素类型
                Type * formalElemType = const_cast<Type *>(formalArrayType->getElementType());
                Type * paramElemType = const_cast<Type *>(paramArrayType->getElementType());

                // 检查元素类型是否相同或兼容
                if (formalElemType->getTypeID() != paramElemType->getTypeID()) {
                    minic_log(LOG_ERROR, "函数(%s)第%d个数组参数元素类型不匹配", funcName.c_str(), (int) i + 1);
                    return false;
                }

                // 元素类型兼容，可以接受
                continue;
            }
        }

        // 普通类型检查
        if (paramType != formalType) {
            minic_log(LOG_ERROR, "函数(%s)第%d个参数类型不匹配", funcName.c_str(), (int) i + 1);
            return false;
        }
    }

    // 返回调用有返回值，则需要分配临时变量，用于保存函数调用的返回值
    Type * type = calledFunction->getReturnType();
    if (!type) {
        minic_log(LOG_ERROR, "函数(%s)返回类型无效", funcName.c_str());
        return false;
    }

    // 创建函数调用指令
    FuncCallInstruction * funcCallInst = new FuncCallInstruction(currentFunc, calledFunction, realParams, type);
    if (!funcCallInst) {
        minic_log(LOG_ERROR, "创建函数调用指令失败");
        return false;
    }

    // 将函数调用指令添加到当前节点的指令列表中
    node->blockInsts.addInst(funcCallInst);

    // 函数调用结果Value保存到node中，可能为空，上层节点可利用这个值
    node->val = funcCallInst;

    return true;
}

/// @brief 语句块（含函数体）AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_block(ast_node * node)
{
    if (!node) {
        minic_log(LOG_ERROR, "语句块节点无效");
        return false;
    }

    // 进入作用域
    if (node->needScope) {
        module->enterScope();
    }

    // 遍历语句块中的每个语句
    for (auto * son: node->sons) {
        if (!son) {
            minic_log(LOG_ERROR, "语句块中的语句节点无效");
            return false;
        }

        // 遍历Block的每个语句，进行显示或者运算
        ast_node * temp = ir_visit_ast_node(son);
        if (!temp) {
            minic_log(LOG_ERROR, "语句处理失败：节点类型(%d)", (int) son->node_type);
            return false;
        }

        // 添加语句的指令到当前块
        node->blockInsts.addInst(temp->blockInsts);

        // 只有表达式语句、return、函数调用等才传递val，普通赋值语句不传递val
        if (temp->val && (son->node_type == ast_operator_type::AST_OP_RETURN ||
                          son->node_type == ast_operator_type::AST_OP_FUNC_CALL)) {
            node->val = temp->val;
        } else {
            node->val = nullptr;
        }
    }

    // 离开作用域
    if (node->needScope) {
        module->leaveScope();
    }

    return true;
}

/// @brief 整数加法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_add(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 加法节点，左结合，先计算左节点，后计算右节点

    // 加法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 加法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * addInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_ADD_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(addInst);

    node->val = addInst;

    return true;
}

/// @brief 整数减法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_sub(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 加法节点，左结合，先计算左节点，后计算右节点

    // 加法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 加法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * subInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_SUB_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(subInst);

    node->val = subInst;

    return true;
}

/// @brief 整数乘法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mul(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 乘法节点，左结合，先计算左节点，后计算右节点

    // 乘法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 乘法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * mulInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_MUL_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(mulInst);

    node->val = mulInst;

    return true;
}

/// @brief 整数除法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_div(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 除法节点，左结合，先计算左节点，后计算右节点

    // 除法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 除法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * divInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_DIV_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(divInst);

    node->val = divInst;

    return true;
}

/// @brief 整数求余AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mod(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 求余节点，左结合，先计算左节点，后计算右节点

    // 求余的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 求余的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * modInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_MOD_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(modInst);

    node->val = modInst;

    return true;
}

/// @brief 赋值AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_assign(ast_node * node)
{
    if (!node || node->sons.size() < 2) {
        minic_log(LOG_ERROR, "赋值语句节点格式错误");
        return false;
    }

    ast_node * son1_node = node->sons[0];
    ast_node * son2_node = node->sons[1];

    if (!son1_node || !son2_node) {
        minic_log(LOG_ERROR, "赋值语句操作数节点无效");
        return false;
    }

    // 赋值节点，自右往左运算
    ast_node * left = ir_visit_ast_node(son1_node);
    if (!left) {
        minic_log(LOG_ERROR, "赋值语句左操作数处理失败");
        return false;
    }
    if (!left->val) {
        minic_log(LOG_ERROR, "赋值语句左操作数值无效");
        return false;
    }
    ast_node * right = ir_visit_ast_node(son2_node);
    if (!right) {
        minic_log(LOG_ERROR, "赋值语句右操作数处理失败");
        return false;
    }
    if (!right->val) {
        minic_log(LOG_ERROR, "赋值语句右操作数值无效");
        return false;
    }
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        minic_log(LOG_ERROR, "当前函数未定义");
        return false;
    }

    Instruction * assignInst = nullptr;
    // 判断左值是否为数组元素
    if (son1_node->node_type == ast_operator_type::AST_OP_ARRAY_ACCESS) {
        // 左值是数组元素，生成store指令
        assignInst = new StoreInstruction(currentFunc, left->val, right->val);
    } else {
        // 普通变量赋值
        assignInst = new MoveInstruction(currentFunc, left->val, right->val);
    }
    if (!assignInst) {
        minic_log(LOG_ERROR, "创建赋值指令失败");
        return false;
    }
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(assignInst);
    node->val = assignInst;
    return true;
}

/// @brief return节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_return(ast_node * node)
{
    ast_node * right = nullptr;
    Function * currentFunc = module->getCurrentFunction();

    // return语句可能没有没有表达式，也可能有，因此这里必须进行区分判断
    if (!node->sons.empty()) {
        ast_node * son_node = node->sons[0];

        // 返回的表达式的指令保存在right节点中
        right = ir_visit_ast_node(son_node);
        if (!right) {
            // 某个变量没有定值
            return false;
        }
    }

    // 返回值存在时则移动指令到node中
    if (right) {
        // 创建临时变量保存IR的值，以及线性IR指令
        node->blockInsts.addInst(right->blockInsts);

        // 返回值赋值到函数返回值变量上
        if (currentFunc->getReturnValue()) {
            node->blockInsts.addInst(new MoveInstruction(currentFunc, currentFunc->getReturnValue(), right->val));
        }

        node->val = right->val;
    } else {
        // 没有返回值，设置为0
        if (currentFunc->getReturnValue()) {
            node->blockInsts.addInst(
                new MoveInstruction(currentFunc, currentFunc->getReturnValue(), module->newConstInt(0)));
        }
        node->val = nullptr;
    }

    // 跳转到函数的尾部出口指令上
    node->blockInsts.addInst(
        new GotoInstruction(currentFunc, static_cast<LabelInstruction *>(currentFunc->getExitLabel())));

    return true;
}

/// @brief 类型叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_type(ast_node * node)
{
    // 不需要做什么，直接从节点中获取即可。

    return true;
}

/// @brief 标识符叶子节点翻译成线性中间IR，变量声明的不走这个语句
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_var_id(ast_node * node)
{
    if (!node || node->name.empty()) {
        minic_log(LOG_ERROR, "变量标识符节点无效");
        return false;
    }

    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        minic_log(LOG_ERROR, "当前函数未定义");
        return false;
    }

    // 首先在当前函数的作用域中查找变量
    for (auto * var: currentFunc->getVarValues()) {
        if (var->getName() == node->name) {
            node->val = var;
            return true;
        }
    }

    // 如果在当前函数中没找到，则在全局作用域中查找
    Value * val = module->findVarValue(node->name);
    if (val) {
        node->val = val;
        return true;
    }

    minic_log(LOG_ERROR, "变量(%s)未定义", node->name.c_str());
    return false;
}

/// @brief 无符号整数字面量叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_uint(ast_node * node)
{
    ConstInt * val;

    // 新建一个整数常量Value
    val = module->newConstInt((int32_t) node->integer_val);

    node->val = val;

    return true;
}

/// @brief 变量声明语句节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_declare_statment(ast_node * node)
{
    if (!node) {
        minic_log(LOG_ERROR, "变量声明语句节点无效");
        return false;
    }

    bool result = true;
    std::set<std::string> declaredVars; // 用于检查变量是否重复声明

    for (auto & child: node->sons) {
        if (!child) {
            minic_log(LOG_ERROR, "变量声明子节点无效");
            continue;
        }

        // 变量声明节点应该包含类型和名称，可能还包含初始化表达式
        if (child->sons.size() < 2) {
            minic_log(LOG_ERROR, "变量声明节点格式错误：子节点数量(%d)小于2", (int) child->sons.size());
            continue;
        }

        // 获取变量名
        ast_node * nameNode = child->sons[1];
        std::string varName;
        if (nameNode->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
            if (!nameNode->sons.empty() && nameNode->sons[0]) {
                varName = nameNode->sons[0]->name;
            }
        } else {
            varName = nameNode->name;
        }
        if (varName.empty()) {
            minic_log(LOG_ERROR, "变量名为空");
            continue;
        }

        // 检查变量是否已经声明
        if (declaredVars.find(varName) != declaredVars.end()) {
            minic_log(LOG_ERROR, "变量(%s)重复声明", varName.c_str());
            return false;
        }

        // 遍历每个变量声明
        result = ir_variable_declare(child);
        if (!result) {
            minic_log(LOG_ERROR, "变量(%s)声明处理失败", varName.c_str());
            break;
        }

        // 记录已声明的变量
        declaredVars.insert(varName);

        // 将变量声明的指令添加到当前节点的指令列表中
        node->blockInsts.addInst(child->blockInsts);
    }

    return result;
}

/// @brief 变量定声明节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_variable_declare(ast_node * node)
{
    if (!node || node->sons.size() < 2) {
        minic_log(LOG_ERROR, "变量声明节点格式错误");
        return false;
    }

    ast_node * typeNode = node->sons[0];
    ast_node * nameNode = node->sons[1];

    std::string varName;
    Type * varType = nullptr;

    if (nameNode->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
        if (!ir_arraytype(nameNode)) {
            minic_log(LOG_ERROR, "数组类型节点处理失败");
            return false;
        }
        if (nameNode->sons.size() < 1 || !nameNode->sons[0]) {
            minic_log(LOG_ERROR, "数组声明节点无变量名");
            return false;
        }
        varName = nameNode->sons[0]->name;
        varType = nameNode->type;
    } else {
        varName = nameNode->name;
        varType = typeNode->type;
    }

    if (varName.empty()) {
        minic_log(LOG_ERROR,
                  "变量名无效: 节点类型=%d, 子节点数量=%d",
                  (int) nameNode->node_type,
                  (int) nameNode->sons.size());
        return false;
    }
    if (!varType) {
        minic_log(LOG_ERROR, "变量类型无效");
        return false;
    }

    Function * currentFunc = module->getCurrentFunction();

    std::string varNameWithDims = "@" + varName;
    std::string typeStr = varType->toString(); // fallback

    if (varType->isArrayType()) {
        const ArrayType * arrType = static_cast<const ArrayType *>(varType);
        const Type * baseType = arrType->getBaseElementType();
        typeStr = baseType ? baseType->toString() : "void";

        std::vector<uint32_t> dims = arrType->getDimensions();
        for (uint32_t dim: dims) {
            varNameWithDims += "[" + std::to_string(dim) + "]";
        }
    }

    std::string declareStr = "declare " + typeStr + " " + varNameWithDims;

    // 调试输出IR声明格式
    minic_log(LOG_DEBUG,
              "IR变量声明格式: %s (文件: %s, 函数: %s, 行: %d)",
              declareStr.c_str(),
              __FILE__,
              __FUNCTION__,
              __LINE__);

    if (!currentFunc) {
        // 全局变量
        Value * var = module->newVarValue(varType, varName);
        if (!var) {
            minic_log(LOG_ERROR, "创建全局变量(%s)失败", varName.c_str());
            return false;
        }
        LabelInstruction * labelInst = new LabelInstruction(nullptr);
        labelInst->setIRName(declareStr);
        node->blockInsts.addInst(labelInst);
        node->val = var;
        return true;
    }

    // 局部变量
    LocalVariable * var = currentFunc->newLocalVarValue(varType, varName);
    if (!var) {
        minic_log(LOG_ERROR, "创建变量(%s)失败", varName.c_str());
        return false;
    }
    node->val = var;

    if (!varName.empty()) {
        declareStr += " ; " + std::to_string(node->line_no) + ":" + varName;
    }

    LabelInstruction * labelInst = new LabelInstruction(currentFunc);
    labelInst->setIRName(declareStr);
    node->blockInsts.addInst(labelInst);

    // 初始化表达式处理
    if (node->sons.size() > 2) {
        ast_node * initNode = node->sons[2];
        if (initNode) {
            ast_node * initValue = ir_visit_ast_node(initNode);
            if (!initValue || !initValue->val) {
                minic_log(LOG_ERROR, "变量(%s)初始化表达式处理失败", varName.c_str());
                return false;
            }
            MoveInstruction * moveInst = new MoveInstruction(currentFunc, var, initValue->val);
            if (!moveInst) {
                minic_log(LOG_ERROR, "创建变量(%s)初始化指令失败", varName.c_str());
                return false;
            }
            node->blockInsts.addInst(initValue->blockInsts);
            node->blockInsts.addInst(moveInst);
        }
    }

    return true;
}

/// @brief 单目负运算符AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_unary_minus(ast_node * node)
{
    ast_node * operand_node = node->sons[0];

    // 先计算操作数
    ast_node * operand = ir_visit_ast_node(operand_node);
    if (!operand) {
        // 操作数计算失败
        return false;
    }

    // 如果操作数是常量，直接计算结果
    if (auto constInt = dynamic_cast<ConstInt *>(operand->val)) {
        int32_t val = constInt->getVal();
        node->val = module->newConstInt(-val);
        return true;
    }

    // 创建一个常量0
    auto zero = new ConstInt(0);

    // 创建减法指令：0 - operand
    BinaryInstruction * subInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_SUB_I,
                                                        zero,
                                                        operand->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(operand->blockInsts);
    node->blockInsts.addInst(subInst);

    node->val = subInst;

    return true;
}

/// @brief 整数小于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_lt(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建小于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_LT,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief 整数大于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_gt(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建大于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_GT,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief 整数大于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_ge(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建大于等于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_GE,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief 整数小于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_le(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建小于等于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_LE,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief 整数等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_eq(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建等于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_EQ,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief 整数不等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_ne(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 遍历左右操作数
    left = ir_visit_ast_node(left);
    right = ir_visit_ast_node(right);

    if (!left || !right) {
        return false;
    }

    // 获取当前函数
    Function * func = module->getCurrentFunction();
    if (!func) {
        return false;
    }

    // 创建不等于比较指令
    BinaryInstruction * inst = new BinaryInstruction(func,
                                                     IRInstOperator::IRINST_OP_ICMP_NE,
                                                     left->val,
                                                     right->val,
                                                     IntegerType::getTypeBool());
    if (!inst) {
        return false;
    }

    // 将指令加入到当前节点的指令列表中
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(inst);

    // 设置当前节点的值为指令的结果
    node->val = inst;

    return true;
}

/// @brief if语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_if(ast_node * node)
{
    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();

    // if语句包含两个子节点：条件表达式和if体
    ast_node * cond_node = node->sons[0];
    ast_node * if_body = node->sons[1];

    // 生成条件表达式的IR
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        return false;
    }

    // 创建三个标签：真体、假体、出口
    LabelInstruction * true_label = new LabelInstruction(currentFunc);
    LabelInstruction * false_label = new LabelInstruction(currentFunc);
    LabelInstruction * exit_label = new LabelInstruction(currentFunc);

    // 添加条件判断和跳转指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(new GotoInstruction(currentFunc, cond->val, true_label, false_label));

    // 添加真体标签和if体
    node->blockInsts.addInst(true_label);
    if (!ir_visit_ast_node(if_body)) {
        return false;
    }
    node->blockInsts.addInst(if_body->blockInsts);

    // 添加跳转到出口的指令
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // 添加假体标签
    node->blockInsts.addInst(false_label);

    // 添加出口标签
    node->blockInsts.addInst(exit_label);

    return true;
}

/// @brief if-else语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_if_else(ast_node * node)
{
    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();

    // if-else语句包含三个子节点：条件表达式、if体和else体
    ast_node * cond_node = node->sons[0];
    ast_node * if_body = node->sons[1];
    ast_node * else_body = node->sons[2];

    // 生成条件表达式的IR
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        return false;
    }

    // 创建三个标签：真体、假体、出口
    LabelInstruction * true_label = new LabelInstruction(currentFunc);
    LabelInstruction * false_label = new LabelInstruction(currentFunc);
    LabelInstruction * exit_label = new LabelInstruction(currentFunc);

    // 添加条件判断和跳转指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(new GotoInstruction(currentFunc, cond->val, true_label, false_label));

    // 添加真体标签和if体
    node->blockInsts.addInst(true_label);
    if (!ir_visit_ast_node(if_body)) {
        return false;
    }
    node->blockInsts.addInst(if_body->blockInsts);

    // 保存if体的返回值到统一的返回值变量
    if (currentFunc->getReturnValue() && if_body->val) {
        node->blockInsts.addInst(new MoveInstruction(currentFunc, currentFunc->getReturnValue(), if_body->val));
    }

    // 添加跳转到出口的指令
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // 添加假体标签和else体
    node->blockInsts.addInst(false_label);
    if (!ir_visit_ast_node(else_body)) {
        return false;
    }

    // 确保else体的指令被正确添加到IR中
    node->blockInsts.addInst(else_body->blockInsts);

    // 合并if/else体的val，保证if-else节点的val不为空
    if (if_body->val) {
        node->val = if_body->val;
    } else if (else_body->val) {
        node->val = else_body->val;
    } else {
        node->val = nullptr;
    }

    // 添加出口标签
    node->blockInsts.addInst(exit_label);

    return true;
}

bool IRGenerator::ir_while(ast_node * node)
{
    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();

    // while语句包含两个子节点：条件表达式和循环体
    ast_node * cond_node = node->sons[0];
    ast_node * body_node = node->sons[1];

    // 创建三个标签：入口、循环体、出口
    LabelInstruction * entryLabel = new LabelInstruction(currentFunc);
    LabelInstruction * bodyLabel = new LabelInstruction(currentFunc);
    LabelInstruction * exitLabel = new LabelInstruction(currentFunc);

    // 将当前循环上下文压入栈
    LoopContext loopCtx = {entryLabel, bodyLabel, exitLabel};
    loopStack.push(loopCtx);

    // 添加入口标签
    node->blockInsts.addInst(entryLabel);

    // 生成条件表达式的IR
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        loopStack.pop();
        return false;
    }

    // 添加条件判断和跳转指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(new GotoInstruction(currentFunc, cond->val, bodyLabel, exitLabel));

    // 添加循环体标签和循环体
    node->blockInsts.addInst(bodyLabel);
    if (!ir_visit_ast_node(body_node)) {
        loopStack.pop();
        return false;
    }
    node->blockInsts.addInst(body_node->blockInsts);

    // 添加跳回入口的指令
    node->blockInsts.addInst(new GotoInstruction(currentFunc, entryLabel));

    // 添加出口标签
    node->blockInsts.addInst(exitLabel);

    // 弹出循环上下文
    loopStack.pop();

    return true;
}

bool IRGenerator::ir_break(ast_node * node)
{
    if (loopStack.empty()) {
        // 错误：break语句不在循环内
        return false;
    }

    // 跳转到当前循环的出口标签
    node->blockInsts.addInst(new GotoInstruction(module->getCurrentFunction(), loopStack.top().exitLabel));
    return true;
}

bool IRGenerator::ir_continue(ast_node * node)
{
    if (loopStack.empty()) {
        // 错误：continue语句不在循环内
        return false;
    }

    // 跳转到当前循环的入口标签
    node->blockInsts.addInst(new GotoInstruction(module->getCurrentFunction(), loopStack.top().entryLabel));
    return true;
}
/// @brief 逻辑与AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_logical_and(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        return false;
    }

    // 创建标签
    LabelInstruction * eval_right_label = new LabelInstruction(currentFunc);
    LabelInstruction * false_label = new LabelInstruction(currentFunc);
    LabelInstruction * exit_label = new LabelInstruction(currentFunc);

    // 创建结果变量
    node->val = module->newVarValue(IntegerType::getTypeInt());

    // 生成左操作数的IR
    ast_node * left_result = ir_visit_ast_node(left);
    if (!left_result) {
        return false;
    }
    node->blockInsts.addInst(left_result->blockInsts);

    // 如果左操作数为假，跳转到 false_label；否则跳转到 eval_right_label
    node->blockInsts.addInst(new GotoInstruction(currentFunc, left_result->val, eval_right_label, false_label));

    // eval_right_label: 计算右操作数
    node->blockInsts.addInst(eval_right_label);
    ast_node * right_result = ir_visit_ast_node(right);
    if (!right_result) {
        return false;
    }
    node->blockInsts.addInst(right_result->blockInsts);

    // 如果右操作数为假，跳转到 false_label
    // 否则直接赋值为 true，并跳转到 exit
    auto true_val = module->newConstInt(1);
    LabelInstruction * assign_true_label = new LabelInstruction(currentFunc);
    node->blockInsts.addInst(new GotoInstruction(currentFunc, right_result->val, assign_true_label, false_label));

    // assign_true_label：两个都为 true，赋值为 1
    node->blockInsts.addInst(assign_true_label);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, true_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // false_label：任一为 false，赋值为 0
    node->blockInsts.addInst(false_label);
    auto false_val = module->newConstInt(0);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, false_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // 统一出口
    node->blockInsts.addInst(exit_label);

    return true;
}

/// @brief 逻辑或AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_logical_or(ast_node * node)
{
    // 获取左右操作数
    ast_node * left = node->sons[0];
    ast_node * right = node->sons[1];

    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        return false;
    }

    // 创建标签
    LabelInstruction * eval_right_label = new LabelInstruction(currentFunc);
    LabelInstruction * true_label = new LabelInstruction(currentFunc);
    LabelInstruction * exit_label = new LabelInstruction(currentFunc);

    // 创建结果变量
    node->val = module->newVarValue(IntegerType::getTypeInt());

    // 生成左操作数的IR
    ast_node * left_result = ir_visit_ast_node(left);
    if (!left_result) {
        return false;
    }
    node->blockInsts.addInst(left_result->blockInsts);

    // 如果左操作数为真，跳转到 true_label；否则跳转到 eval_right_label
    node->blockInsts.addInst(new GotoInstruction(currentFunc, left_result->val, true_label, eval_right_label));

    // eval_right_label: 计算右操作数
    node->blockInsts.addInst(eval_right_label);
    ast_node * right_result = ir_visit_ast_node(right);
    if (!right_result) {
        return false;
    }
    node->blockInsts.addInst(right_result->blockInsts);

    // 如果右操作数为真，跳转到 true_label
    // 否则直接赋值为 false，并跳转到 exit
    auto false_val = module->newConstInt(0);
    LabelInstruction * assign_false_label = new LabelInstruction(currentFunc);
    node->blockInsts.addInst(new GotoInstruction(currentFunc, right_result->val, true_label, assign_false_label));

    // assign_false_label：两个都为 false，赋值为 0
    node->blockInsts.addInst(assign_false_label);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, false_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // true_label：任一为 true，赋值为 1
    node->blockInsts.addInst(true_label);
    auto true_val = module->newConstInt(1);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, true_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // 统一出口
    node->blockInsts.addInst(exit_label);

    return true;
}

/// @brief 逻辑非AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_logical_not(ast_node * node)
{
    // 获取操作数
    ast_node * operand = node->sons[0];

    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        return false;
    }

    // 创建标签
    LabelInstruction * true_label = new LabelInstruction(currentFunc);
    LabelInstruction * false_label = new LabelInstruction(currentFunc);
    LabelInstruction * exit_label = new LabelInstruction(currentFunc);

    // 创建结果变量
    node->val = module->newVarValue(IntegerType::getTypeInt());

    // 生成操作数的IR
    ast_node * operand_result = ir_visit_ast_node(operand);
    if (!operand_result) {
        return false;
    }
    node->blockInsts.addInst(operand_result->blockInsts);

    // 注意：这里翻转了true_label和false_label的顺序
    // 如果操作数为真，跳转到false_label；否则跳转到true_label
    node->blockInsts.addInst(new GotoInstruction(currentFunc, operand_result->val, false_label, true_label));

    // true_label：操作数为假，结果为1
    node->blockInsts.addInst(true_label);
    auto true_val = module->newConstInt(1);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, true_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // false_label：操作数为真，结果为0
    node->blockInsts.addInst(false_label);
    auto false_val = module->newConstInt(0);
    node->blockInsts.addInst(new MoveInstruction(currentFunc, node->val, false_val));
    node->blockInsts.addInst(new GotoInstruction(currentFunc, exit_label));

    // 统一出口
    node->blockInsts.addInst(exit_label);

    return true;
}

/// @brief 数组类型节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_arraytype(ast_node * node)
{
    // 安全检查
    if (!node) {
        minic_log(LOG_ERROR, "数组类型节点为空");
        return false;
    }

    minic_log(LOG_DEBUG, "处理数组类型节点，节点类型ID: %d", static_cast<int>(node->node_type));

    // 数组类型节点应该有至少两个子节点：变量名和维度
    if (node->sons.size() < 2) {
        minic_log(LOG_ERROR, "数组类型节点结构错误，子节点数量: %zu", node->sons.size());
        return false;
    }

    // 获取变量名节点
    ast_node * name_node = node->sons[0];
    if (!name_node) {
        minic_log(LOG_ERROR, "数组变量名节点为空");
        return false;
    }

    minic_log(LOG_DEBUG,
              "数组变量名节点类型ID: %d, 节点名称: '%s'",
              static_cast<int>(name_node->node_type),
              name_node->name.empty() ? "<空>" : name_node->name.c_str());

    // 保存变量名以备后用
    std::string arrayName;
    // 检查节点类型是否为变量ID叶子节点
    if (name_node->node_type == ast_operator_type::AST_OP_LEAF_VAR_ID) {
        // 对于变量ID叶子节点，name字段就是变量名
        arrayName = name_node->name;
        minic_log(LOG_DEBUG, "从AST_OP_LEAF_VAR_ID节点获取变量名: '%s'", arrayName.c_str());
    } else {
        // 尝试直接获取name字段
        arrayName = name_node->name;
        minic_log(LOG_DEBUG, "从节点name字段获取变量名: '%s'", arrayName.c_str());

        // 如果name字段为空但节点有值，尝试其他方式获取
        if (arrayName.empty()) {
            // 尝试手动查看节点的其他属性
            minic_log(LOG_DEBUG,
                      "尝试其他方式获取变量名: integer_val=%u, line_no=%ld",
                      name_node->integer_val,
                      name_node->line_no);
        }
    }

    if (arrayName.empty()) {
        minic_log(LOG_ERROR, "数组变量名为空");
        return false;
    }

    // 保存变量名到节点自身
    node->name = arrayName;
    minic_log(LOG_DEBUG, "已将变量名'%s'保存到数组类型节点", arrayName.c_str());

    // 获取维度节点
    ast_node * dimen_node = node->sons[1];
    if (!dimen_node) {
        minic_log(LOG_ERROR, "数组维度节点为空");
        return false;
    }

    minic_log(LOG_DEBUG, "数组维度节点类型ID: %d", static_cast<int>(dimen_node->node_type));

    // 处理维度
    ast_node * processed_dimen = ir_visit_ast_node(dimen_node);
    if (!processed_dimen) {
        minic_log(LOG_ERROR, "处理数组维度失败");
        return false;
    }

    // 从当前作用域或变量声明获取基本类型
    Type * base_type = IntegerType::getTypeInt(); // 默认为整型

    // 存储所有维度的大小，用于调试和类型创建
    std::vector<uint32_t> dimensions;

    // 从维度节点获取每个维度的大小
    minic_log(LOG_DEBUG, "处理数组维度，维度数量: %zu", processed_dimen->sons.size());
    for (size_t i = 0; i < processed_dimen->sons.size(); i++) {
        ast_node * dim_size_node = processed_dimen->sons[i];
        if (!dim_size_node) {
            minic_log(LOG_ERROR, "第%zu维度节点为空", i + 1);
            return false;
        }

        if (!dim_size_node->val) {
            dim_size_node = ir_visit_ast_node(dim_size_node);
            if (!dim_size_node || !dim_size_node->val) {
                minic_log(LOG_ERROR, "处理第%zu维度大小失败", i + 1);
                return false;
            }
        }

        // 获取维度大小
        ConstInt * const_size = dynamic_cast<ConstInt *>(dim_size_node->val);
        if (!const_size) {
            minic_log(LOG_ERROR, "第%zu维度不是常量整数", i + 1);
            return false;
        }

        uint32_t size = const_size->getVal();
        dimensions.push_back(size);
        minic_log(LOG_DEBUG, "处理数组维度[%zu]: %d", i, size);
    }

    // 从内向外构建多维数组类型（从最里层开始）
    // 例如: int[10][20][30] 应该先构建 int[30]，然后是 int[30][20]，最后是 int[30][20][10]
    Type * array_type = base_type;
    for (int i = dimensions.size() - 1; i >= 0; i--) {
        const ArrayType * arr_type = ArrayType::get(array_type, dimensions[i]);
        if (!arr_type) {
            minic_log(LOG_ERROR, "创建数组类型失败，维度[%d]=%d", i, dimensions[i]);
            return false;
        }

        // 在需要非const指针时，使用 const_cast
        array_type = const_cast<ArrayType *>(arr_type);
        minic_log(LOG_DEBUG,
                  "创建数组类型，维度[%d]=%d, 类型字符串: %s",
                  i,
                  dimensions[i],
                  array_type->toString().c_str());
    }

    // 设置节点的类型
    node->type = array_type;

    minic_log(LOG_DEBUG,
              "完成数组类型处理，类型ID: %d, 类型字符串: %s",
              static_cast<int>(array_type->getTypeID()),
              array_type->toString().c_str());

    return true;
}

/// @brief 数组维度节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_arraydimen(ast_node * node)
{
    // 安全检查
    if (!node) {
        minic_log(LOG_ERROR, "数组维度节点为空");
        return false;
    }

    // 数组维度应该是一个表达式，通常是常量整数
    if (node->node_type == ast_operator_type::AST_OP_LEAF_LITERAL_UINT) {
        // 如果是常量整数节点，直接创建常量值
        node->val = module->newConstInt((int32_t) node->integer_val);
        return true;
    }

    // 如果有子节点，处理子表达式
    if (!node->sons.empty()) {
        ast_node * expr = ir_visit_ast_node(node->sons[0]);
        if (!expr || !expr->val) {
            minic_log(LOG_ERROR, "处理数组维度表达式失败");
            return false;
        }

        node->val = expr->val;
        node->blockInsts.addInst(expr->blockInsts);
    } else {
        minic_log(LOG_ERROR, "无法确定数组维度");
        return false;
    }

    return true;
}

/// @brief 数组下标访问节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_arrayaccess(ast_node * node)
{
    // 安全检查
    if (!node || node->sons.size() < 2) {
        minic_log(LOG_ERROR, "数组下标访问节点结构错误");
        return false;
    }

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        minic_log(LOG_ERROR, "数组访问处理时当前函数为空");
        return false;
    }

    // 处理数组变量
    ast_node * array_node = ir_visit_ast_node(node->sons[0]);
    if (!array_node || !array_node->val) {
        minic_log(LOG_ERROR, "处理数组变量失败");
        return false;
    }

    // 处理索引表达式
    ast_node * index_node = ir_visit_ast_node(node->sons[1]);
    if (!index_node || !index_node->val) {
        minic_log(LOG_ERROR, "处理数组索引失败");
        return false;
    }

    // 获取数组基址和类型
    Value * array_base = array_node->val;
    Type * array_type = array_base->getType();

    if (!array_type) {
        minic_log(LOG_ERROR, "数组类型为空");
        return false;
    }

    minic_log(LOG_DEBUG, "数组访问: 类型ID=%d", static_cast<int>(array_type->getTypeID()));

    // 确定元素类型和大小
    Type * element_type = nullptr;
    int32_t elem_size = 4; // 默认4字节

    if (array_type->isArrayType()) {
        const ArrayType * arrayType = static_cast<const ArrayType *>(array_type);
        element_type = arrayType->getElementType();
        elem_size = element_type->getSize();
    } else if (array_type->isPointerType()) {
        // 处理指针类型
        const PointerType * ptrType = static_cast<const PointerType *>(array_type);
        element_type = const_cast<Type *>(ptrType->getPointeeType());
        elem_size = element_type->getSize();
    } else {
        minic_log(LOG_ERROR, "变量不是数组或指针类型");
        return false;
    }

    // 步骤1: 计算偏移量 (index * 元素大小)
    BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                        IRInstOperator::IRINST_OP_MUL_I,
                                                        index_node->val,
                                                        module->newConstInt(elem_size),
                                                        IntegerType::getTypeInt());

    // 步骤2: 计算元素地址 (base + offset)
    // 创建指针类型，指向元素类型
    const PointerType * ptr_type = PointerType::get(element_type);
    Type * address_type = const_cast<PointerType *>(ptr_type);

    BinaryInstruction * addInst = new BinaryInstruction(currentFunc,
                                                        IRInstOperator::IRINST_OP_ADD_I,
                                                        array_base,
                                                        mulInst,
                                                        address_type // 使用指针类型而不是元素类型
    );

    // 步骤3: 根据上下文决定是返回地址还是加载值
    LoadInstruction * loadInst = nullptr;

    // 只有在数组访问作为右值时才创建加载指令
    // 判断该节点是否是赋值语句的左侧
    bool isLeftValueOfAssign = false;
    if (node->parent && node->parent->node_type == ast_operator_type::AST_OP_ASSIGN) {
        // 检查是否是左值
        if (!node->parent->sons.empty() && node->parent->sons[0] == node) {
            isLeftValueOfAssign = true;
        }
    }

    if (!isLeftValueOfAssign) {
        // 作为右值使用，需要加载元素值
        loadInst = new LoadInstruction(currentFunc, addInst, element_type);
    }

    // 收集指令
    node->blockInsts.addInst(array_node->blockInsts);
    node->blockInsts.addInst(index_node->blockInsts);
    node->blockInsts.addInst(mulInst);
    node->blockInsts.addInst(addInst);

    if (loadInst) {
        node->blockInsts.addInst(loadInst);
        node->val = loadInst; // 访问结果是加载的值
    } else {
        node->val = addInst; // 访问结果是地址
    }

    // 保存元素类型
    node->type = element_type;

    return true;
}
