# unique_ptr

---

unique_ptr拥有对持有对象的唯一所有权，即两个unique_ptr不能同时指向同一个对象。unique_ptr不能被复制，只能被移动。

```cpp
unique_ptr<T> a1(new A());
unique_ptr<T> a2 = a1;// error
unique_ptr<T> a3 = move(a1);// 正确，所有权转移后，a1不再拥有任何指针
```

> 智能指针有一个通用的规则，就是->表示用于调用指针原有的方法，而.则表示调用智能指针本身的方法。

unique_ptr本身拥有的方法主要包括：

1. get()获取其保存的原生指针，尽量不要使用
2. bool()判断是否拥有指针
3. release()释放所管理指针的所有权，返回原生指针，但并不销毁原生指针
4. reset()释放并销毁原生指针，如果参数是一个新指针，将管理这个新指针

```cpp
unique_ptr<A> a1(new A());
A *origin_a = a1.get();// 尽量不要暴露原生指针

if (a1) {}// 第2点的bool是这样用的
unique_ptr<A> a2(a1.release());// 常见用法，转移拥有权
a2.reset(new A());// 释放并销毁原有对象，持有一个新对象
a2.reset();// 释放并销毁原有对象，等同于下面的写法
a2 = nullptr;// 释放并销毁原有对象
```

## 使用场景

unique_ptr由于没有引用计数，性能较好

1. 在对象内部使用
2. 在方法(函数)内部使用