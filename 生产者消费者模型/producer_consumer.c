#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
void err_thread(int ret, char *str){
	if (ret != 0){
		fprintf(stderr, "%s:%s\n", str, strerror(ret));
		pthread_exit(NULL);
	}
}
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//定义/初始化一个互斥量
pthread_cond_t has_data = PTHREAD_COND_INITIALIZER;//定义/初始化一个条件变量

struct msg {
  int data;
  struct msg* next;
};

struct msg* head;

void* producer() {
  while (1) {
    struct msg* mp = malloc(sizeof(struct msg));
    mp->data = rand() % 1000 + 1;

    pthread_mutex_lock(&mutex);
    mp->next = head;
    head = mp;
    pthread_mutex_unlock(&mutex);

    printf("--produce: %d\n", mp->data);

    pthread_cond_signal(&has_data);

    sleep(rand() % 4);
  }
  return NULL;
}

void* comsumer() {
  while (1) {
    struct msg* mp;
    pthread_mutex_lock(&mutex);
    if (head == NULL) {
      pthread_cond_wait(&has_data, &mutex);
    }
    mp = head;
    head = head->next;
    pthread_mutex_unlock(&mutex);

    printf("---------comsume: %d\n", mp->data);
    free(mp);

    sleep(rand() % 3);
  }
  return NULL;
}

int main() {
  int ret;
  pthread_t pid, cid;
  srand(time(NULL));// srand是随机数发生器的初始化函数
  
  ret = pthread_create(&pid, NULL, producer, NULL);
  if (ret != 0) err_thread(ret, "creating producer error...");
  ret = pthread_create(&cid, NULL, comsumer, NULL);
  if (ret != 0) err_thread(ret, "creating comsumer error...");

  pthread_join(pid, NULL);
  pthread_join(cid, NULL);

  return 0;
}