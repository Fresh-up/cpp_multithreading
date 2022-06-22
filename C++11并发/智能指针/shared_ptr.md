# shared_ptr

---

多个shared_ptr可以共同拥有同一个原生指针的所有权。

```cpp
shared_ptr<A> a1(new A());// 使用这种方式创建shared_ptr会进行两次内存分配
// 建议用make_shared<A>()
shared_ptr<A> a1 = make_shared<A>();
shared_ptr<A> a2 = a1;// 正确
```

shared_ptr通过引用计数的方式管理指针，当引用计数为0时会销毁拥有的原生对象。

shared_ptr本身拥有的方法主要包括：

1. get()获取其保存的原生指针，尽量不要使用
2. bool()判断是否拥有指针
3. unique()判断引用计数是否为1，是则true,否则false
4. use_count()返回引用计数大小
5. reset()释放并销毁原生指针，如果有参数，则管理新指针

```cpp
shared_ptr<A> a1 = make_shared<A>();
A *origin_a = a1.get();// 尽量不要暴露原生指针

if (a1.unique()) {
    // 如果返回true,引用计数为1
}

long long a1_use_count = a1.use_count();// 返回引用计数
```

## 使用场景

一个对象要被多个类同时使用的时候，用shared_ptr

如果采用this指针重新构造shared_ptr是不行的，因为重新创建的shared_ptr与当前对象的shared_ptr没有关系，没有增加当前对象的引用计数。这将导致任何一个shared_ptr计数为0时提前释放了对象，后续操作这个释放的对象都会导致程序异常。

这时就要引入`shared_from_this`.对象集成了`enable_shared_from_this`后，可以通过`shared_from_this()`获取当前对象的shared_ptr指针。

```cpp
class A: public endable_shared_from_this<A> {
    // ...

public:
    void new_D() {
        // 错误方式，用this指针重新构造shared_ptr,将导致二次释放当前对象
        shared_ptr<A> this_shared_ptr1(this);
        unique_ptr<D> d1(new D(this_shared_ptr1));
        // 正确方式
        shared_ptr<A> this_shared_ptr2 = shared_from_this();
        unique_ptr<D> d2(new D(this_shared_ptr2));
    }
};
```