#include "threadPool.h"
int threadPoolInit(threadPool_t *pThreadPool, int workerNum){
    bzero(pThreadPool,sizeof(threadPool_t));
    pThreadPool->threadNum = workerNum;
    pThreadPool->tidArr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    
    pThreadPool->taskQueue.pFront = NULL;
    pThreadPool->taskQueue.pRear = NULL;
    pThreadPool->taskQueue.size = 0;
    
    pthread_mutex_init(&pThreadPool->mutex,NULL);
    pthread_cond_init(&pThreadPool->cond,NULL);

    pThreadPool->isThreadPoolRunning = 1;
}