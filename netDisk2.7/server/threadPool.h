#include <func.h>
#include "taskQueue.h"
#ifndef __THREADPOOL__
#define __THREADPOOL__
typedef struct threadPool_s{
    pthread_t *tidArr;//所有的子线程的tid构成的数据
    int threadNum;//子线程的数量
    taskQueue_t taskQueue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int isThreadPoolRunning;
    int sockFd;
} threadPool_t;
int threadPoolInit(threadPool_t *pThreadPool, int workerNum);
#endif