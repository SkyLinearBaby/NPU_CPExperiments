///
/// @file ArrayType.h
/// @brief 数组类型描述类
///
#pragma once
#include <vector>

#include "Type.h"
#include "StorageSet.h"

///
/// @brief 数组类型
///
class ArrayType : public Type {
    ///
    /// @brief Hash用结构体，提供Hash函数
    ///
    struct ArrayTypeHasher final {
        size_t operator()(const ArrayType & type) const noexcept
        {
            size_t h1 = std::hash<const Type *>{}(type.getElementType());
            size_t h2 = std::hash<uint32_t>{}(type.getNumElements());
            return h1 ^ (h2 << 1);
        }
    };

    ///
    /// @brief 判断两者相等的结构体，提供等于函数
    ///
    struct ArrayTypeEqual final {
        bool operator()(const ArrayType & lhs, const ArrayType & rhs) const noexcept
        {
            return lhs.getElementType() == rhs.getElementType() && lhs.getNumElements() == rhs.getNumElements();
        }
    };

public:
    ///
    /// @brief 构造函数
    /// @param elemType 元素类型
    /// @param numElements 元素数量
    ///
    ArrayType(Type * elemType, uint32_t numElements) : Type(ArrayTyID), elementType(elemType), numElements(numElements)
    {
        // 计算元素大小
        if (elemType) {
            elementSize = elemType->getSize();
            // 处理0维数组（作为函数参数的特殊情况）
            if (numElements == 0) {
                // 数组作为函数参数时，第一维为0
                totalSize = 4; // 只存储指针大小
            } else {
                // 正常数组
                totalSize = elementSize * numElements;
            }
        } else {
            elementSize = 0;
            totalSize = 0;
        }
    }

    ///
    /// @brief 获取元素类型
    /// @return 元素类型
    ///
    Type * getElementType() const
    {
        return elementType;
    }

    ///
    /// @brief 获取元素数量
    /// @return 元素数量
    ///
    uint32_t getNumElements() const
    {
        return numElements;
    }

    ///
    /// @brief 获取元素大小（字节）
    /// @return 元素大小
    ///
    uint32_t getElementSize() const
    {
        return elementSize;
    }

    ///
    /// @brief 获取总大小（字节）
    /// @return 总大小
    ///
    int32_t getSize() const override
    {
        return totalSize;
    }

    ///
    /// @brief 获取类型的字符串表示
    /// @return 类型字符串
    ///
    std::string toString() const override;

    ///
    /// @brief 创建或获取数组类型
    /// @param elemType 元素类型
    /// @param numElements 元素数量
    /// @return 数组类型
    ///
    static const ArrayType * get(Type * elemType, uint32_t numElements)
    {
        static StorageSet<ArrayType, ArrayTypeHasher, ArrayTypeEqual> storageSet;
        return storageSet.get(elemType, numElements);
    }
    const Type * getBaseElementType() const;
    std::vector<uint32_t> getDimensions() const;

private:
    Type * elementType;   // 元素类型
    uint32_t numElements; // 元素数量
    uint32_t elementSize; // 元素大小（字节）
    int32_t totalSize;    // 总大小（字节）
};