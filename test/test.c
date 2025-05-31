// 测试用例：函数定义、形参、实参和返回类型

// 计算两个整数的和
int add(int a, int b) {
    return a + b;
}

// 打印一个整数
void print(int x) {
    // void函数体
}

// 计算阶乘
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// 主函数
int main() {
    // 测试函数调用和参数传递
    int sum = add(5, 3);
    print(sum);

    // 测试递归函数调用
    int result = factorial(5);
    print(result);

    return 0;
}
