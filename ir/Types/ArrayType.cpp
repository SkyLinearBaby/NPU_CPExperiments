#include "ArrayType.h"
#include <vector>

// 输出形如：i32[10][20]（类型 + 维度，适合调试）
std::string ArrayType::toString() const
{
    std::vector<uint32_t> dims;
    const Type * cur = this;
    const Type * base = nullptr;
    while (cur && cur->isArrayType()) {
        auto arr = static_cast<const ArrayType *>(cur);
        dims.push_back(arr->getNumElements());
        cur = arr->getElementType();
    }
    base = cur;
    std::string s = base ? base->toString() : "void";
    for (auto it = dims.rbegin(); it != dims.rend(); ++it) {
        s += "[" + std::to_string(*it) + "]";
    }
    return s;
}

// 获取最底层基本类型（如 i32，而不是 i32[]）
const Type * ArrayType::getBaseElementType() const
{
    const Type * cur = this;
    while (cur && cur->isArrayType()) {
        cur = static_cast<const ArrayType *>(cur)->getElementType();
    }
    return cur;
}

// 获取所有维度列表，例如 int a[3][4] 返回 {3, 4}
std::vector<uint32_t> ArrayType::getDimensions() const
{
    std::vector<uint32_t> dims;
    const Type * cur = this;
    while (cur && cur->isArrayType()) {
        auto arr = static_cast<const ArrayType *>(cur);
        dims.push_back(arr->getNumElements());
        cur = arr->getElementType();
    }
    return dims;
}
