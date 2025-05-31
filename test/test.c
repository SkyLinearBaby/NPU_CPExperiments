// 简单测试用例：多形参函数定义和调用、void返回值和int返回值

// void函数，无参数
void emptyFunction() {
  // 空函数体
}

// void函数，单参数
void printValue(int x) {
  int temp;
  temp = x;  // 简单使用参数，不调用内置函数
}

// int函数，两个参数
int add(int a, int b) {
  return a + b;
}

// int函数，三个参数
int calculate(int a, int b, int c) {
  int result;
  result = a + b;
  return result + c;
}

// main函数，测试函数调用
int main() {
  int x;
  int y;
  int result;
  
  // 设置初始值
  x = 10;
  y = 20;
  
  // 调用void函数
  emptyFunction();
  printValue(x);
  
  // 调用int函数
  result = add(x, y);
  
  // 测试多参数函数
  result = calculate(x, y, x);

  // main函数会自动返回0
  return 0;
}