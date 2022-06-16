#include <iostream>
#include <thread>
#include <condition_variable>
#include <time.h>
#include <chrono>
#include <stdlib.h>
#include <mutex>
using namespace std;

struct msg {
  int data;
  msg* next;
  msg(int a) : data(a) {}
};

mutex lck;
condition_variable cond;

msg* head;

void producer() {
  while (1) {
    msg* mp = new msg(rand() % 1000 + 1);

    lck.lock();
    mp->next = head;
    head = mp;
    lck.unlock();

    cout << "--produce: " << mp->data << endl;
    cond.notify_one();
    this_thread::sleep_for(chrono::seconds(rand() % 3));

  }
}

void comsumer() {
  while (1) {
    msg* mp;

    unique_lock<mutex> lk(lck);
    if (head == nullptr) cond.wait(lk);

    mp = head;
    head = head->next;
    lk.unlock();

    cout << "----------comsume: " << mp->data << endl;
    delete mp;

    this_thread::sleep_for(chrono::seconds(rand() % 3));
  }
}

int main() {
  srand(time(NULL));
  
  thread t1(producer);
  thread t2(comsumer);

  t1.join();
  t2.join();
}