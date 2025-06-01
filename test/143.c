// 全局变量声明
int global_counter = 0;
int global_sum = 0;

// void 函数定义
void update_global_variables(int value) {
    // 增加计数器
    global_counter = global_counter + 1;

    // 更新总和
    global_sum = global_sum + value;

    // 如果总和超过100，重置为0
    if (global_sum > 100) {
        global_sum = 0;
    }

    // 打印当前状态
    putint(global_counter);
    putch(32); // 空格的 ASCII 码
    putint(global_sum);
    putch(10); // 换行的 ASCII 码
}

// 主函数
int main() {
    // 多次调用 void 函数
    update_global_variables(30);
    update_global_variables(50);
    update_global_variables(40);
    update_global_variables(20);
    update_global_variables(10);

    // 最后打印全局变量的值
    putint(global_counter);
    putch(32); // 空格的 ASCII 码
    putint(global_sum);

    return 0;
}
// 1 30
//  2 80
//  3 0
//  4 20
//  5 30
// 5 30
// 0
