#include "ArrayType.h"
#include <vector>

std::string ArrayType::toString() const
{
    // 找到最内层的非数组类型
    Type * currentType = elementType;
    while (currentType->isArrayType()) {
        currentType = static_cast<const ArrayType *>(currentType)->getElementType();
    }

    // 只返回基本类型字符串，不包含数组维度
    return currentType->toString();
}