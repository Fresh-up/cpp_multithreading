# weak_ptr

---

`weak_ptr` 比较特殊，它主要是为了配合`shared_ptr`而存在的。就像它的名字一样，它本身是一个弱指针，因为它本身是不能直接调用原生指针的方法的。如果想要使用原生指针的方法，需要将其先转换为一个`shared_ptr`。那`weak_ptr`存在的意义到底是什么呢？

由于`shared_ptr`是通过引用计数来管理原生指针的，那么最大的问题就是循环引用（比如 a 对象持有 b 对象，b 对象持有 a 对象），这样必然会导致内存泄露。而`weak_ptr`不会增加引用计数，因此将循环引用的一方修改为弱引用，可以避免内存泄露。

`weak_ptr`可以通过一个`shared_ptr`创建。

```cpp
shared_ptr<A> a1 = make_shared<A>();
weak_ptr<A> weak_a1 = a1;// 不增加引用次数
```

`weak_ptr`本身拥有的方法有：

1. expired()判断所指向的原生指针是否被释放，如果被释放了返回true, 否则返回false
2. use_count()返回引用次数
3. lock()返回shared_ptr, 如果原生指针没有被释放，则返回一个非空的shared_ptr, 否则返回一个空的shared_ptr
4. reset()将本身置空

```cpp
shared_ptr<A> a1 = make_shared<A>();
weak_ptr<A> weak_a1 = a1;

if (weak_a1.expired()) {
    // 如果为true, weak_a1对应的原生指针已经被释放
}
long long a1_use_count = weak_a1.use_count();

if (shared_ptr<A> shared_a = weak_a1.lock()) {
    // 此时可以通过shared_a进行原生指针的方法调用
}

weak_a1.reset();// 将weak_a1置空
```

## 使用场景

在类内定义weak_ptr, 在类内方法用shared_ptr作为参数，然后赋值给weak_ptr