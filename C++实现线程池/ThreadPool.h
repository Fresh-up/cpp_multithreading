#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
using namespace std;

class ThreadPool {
private:

  class ThreadWorker {// 内置线程工作类
  private:
    int m_id;// 工作id
    ThreadPool *m_pool;// 所属线程池
  public:
    // 构造函数
    ThreadWorker(ThreadPool *pool, const int id):m_pool(pool), m_id(id){}

    // 重载()操作
    void operator()() {
      function<void()> func;

      bool dequeued;// 判断是否成功取出任务队列中的任务

      while (!m_pool->m_shutdown) {
        // 判断线程池是否关闭，没有关闭则从任务队列中循环提取任务
        {
          //为线程环境加锁，访问工作线程的休眠和唤醒
          unique_lock<mutex> lock(m_pool->m_conditional_mutex);

          // 如果任务队列为空，阻塞当前线程
          if (m_pool->m_queue.empty()) {
            m_pool->m_conditional_lock.wait(lock);// 等待条件变量通知，开启线程
          }

          // 取出任务队列中的任务
          dequeued = m_pool->m_queue.dequeue(func);
        }

        // 如果成功取出，执行工作函数
        if (dequeued) func();
      }
    }
  };

  bool m_shutdown;// 线程池是否关闭

  template <typename T>
  class safequeue {
    queue<T> mq;
    mutex mmutex;
  public:
    safequeue() {}
    safequeue(safequeue &&other) {}
    ~safequeue() {}

    bool empty() {
      unique_lock<mutex> lock(mmutex);
      return mq.empty();
    }

    int size() {
      unique_lock<mutex> lock(mmutex);
      return mq.size();
    }

    void enqueue(T &t) {
      unique_lock<mutex> lock(mmutex);
      mq.emplace(t);
    }

    bool dequeue(T &t) {
      unique_lock<mutex> lock(mmutex);

      if (mq.empty()) return false;
      t = move(mq.front());

      mq.pop();

      return true;
    }
  };

  safequeue<function<void()>> m_queue;// 任务队列
  vector<thread> m_threads;// 工作线程队列
  mutex m_conditional_mutex;// 线程休眠锁互斥量
  condition_variable m_conditional_lock;// 线程环境锁，可以让线程处于休眠或唤醒状态

public:
  // 线程池构造函数
  ThreadPool(const int n_threads = 16) 
    : m_threads(vector<thread>(n_threads)), m_shutdown(false) {}
  
  // = delete是禁用该函数
  ThreadPool(const ThreadPool &) = delete;// 禁用拷贝构造

  ThreadPool(ThreadPool &&) = delete;// 禁用移动构造

  void init() {
    for (int i = 0; i < m_threads.size(); ++i) {
      m_threads.at(i) = thread(ThreadWorker(this, i));// 分配工作线程
    }
  }

  void shutdown() {
    m_shutdown = true;
    m_conditional_lock.notify_all();// 唤醒所有工作线程

    for (int i = 0; i < m_threads.size(); ++i) {
      if (m_threads.at(i).joinable()) {
        // 判断线程是否在等待
        m_threads.at(i).join();
      }
    }
  }

  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args) -> future<decltype(f(args...))> {
    function<decltype(f(args...))()> func = bind(forward<F>(f), forward<Args>(args)...);
    auto task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(func);

    function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };

    m_queue.enqueue(wrapper_func);
    m_conditional_lock.notify_one();
    return task_ptr->get_future();
  }

};