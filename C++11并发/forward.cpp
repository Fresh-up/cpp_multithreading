#include <iostream>
#include <utility>
using namespace std;

template<typename T>
void print(T& t){
    cout << "lvalue" << endl;
}
template<typename T>
void print(T&& t){
    cout << "rvalue" << endl;
}

template<typename T>
void TestForward(T && v){// TestForward支持右值的参数
    print(v);
    print(std::forward<T>(v));
    print(std::move(v));
}

int main(){
    TestForward(1);// 输出l,r,r
    int x = 1;
    print(forward<int>(x));// x原来是个右值, 输出rvalue
    TestForward(forward<int>(x));// 输出lvalue,rvalue,rvalue
    TestForward(x);// 输出l,l,r
    TestForward(move(x));// 输出l,r,r
    return 0;
}
// 一个接受右值形参的函数，当传入是一个左值时，在函数内就会把它当作左值处理
// 一个接受右值形参的函数，当传入一个右值时，在函数内部会为该函数命名，所以该右值变为了左值
// 通过forward可以让原来是右值的实参还是作为右值