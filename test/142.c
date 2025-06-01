// 函数定义
int complex_calculation(int a, int b, int c, int d, int e, int f) {
    int sum;
    int product;

    sum = a + b + c + d + e + f;
    product = a * b * c * d * e * f;

    // 如果和大于100，返回和
    if (sum > 100) {
        return sum;
    }

    // 如果积大于10000，返回积
    if (product > 10000) {
        return product;
    }

    // 如果a是最大的，返回a
    if (a >= b && a >= c && a >= d && a >= e && a >= f) {
        return a;
    }

    // 如果b和c的和大于其他任意两个数的和，返回b+c
    if ((b + c) > (a + d) && (b + c) > (a + e) && (b + c) > (a + f) &&
        (b + c) > (d + e) && (b + c) > (d + f) && (b + c) > (e + f)) {
        return b + c;
    }

    // 如果d、e、f中有两个数是偶数，返回这两个偶数的积
    int even_count = 0;
    int even_product = 1;
    if (d % 2 == 0) {
        even_count = even_count + 1;
        even_product = even_product * d;
    }
    if (e % 2 == 0) {
        even_count = even_count + 1;
        even_product = even_product * e;
    }
    if (f % 2 == 0) {
        even_count = even_count + 1;
        even_product = even_product * f;
    }
    if (even_count >= 2) {
        return even_product;
    }

    // 其他情况，返回和与积的差
    return sum - product;
}

// 主函数
int main()
{
    int result;

    // 调用函数并传递六个整数参数
    result = complex_calculation(10, 20, 30, 2, 4, 5);

    // 输出结果
    putint(result);

    return 0;
}
//240000
// 0
