// 函数定义
int calculate_with_conditions(int a, int b, int c, int d, int e) {
    int sum = a + b + c + d + e;

    // 如果和大于100，返回和
    if (sum > 100) {
        return sum;
    }

    int product = a * b * c * d * e;

    // 如果积大于1000，返回积
    if (product > 1000) {
        return product;
    }

    // 如果a是最大的，返回a
    if (a >= b && a >= c && a >= d && a >= e) {
        return a;
    }

    // 其他情况，返回和与积的差
    return sum - product;
}

// 主函数
int main()
{
    int result;

    // 调用函数并传递五个整数参数
    result = calculate_with_conditions(5, 4, 3, 2, 1);

    // 输出结果
    putint(result);

    return 0;
}
// 5
//0