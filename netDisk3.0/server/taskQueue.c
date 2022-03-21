#include <func.h>
#include "taskQueue.h"
int enQueue(taskQueue_t *pTaskQueue, int netFd){
    task_t *pTask = (task_t *)calloc(1,sizeof(task_t));
    pTask->netFd = netFd;
    if(pTaskQueue->size == 0){
        pTaskQueue->pFront = pTask;
        pTaskQueue->pRear = pTask;
    }
    else{
        pTaskQueue->pRear->pNext = pTask;
        pTaskQueue->pRear = pTask;
    }
    ++pTaskQueue->size;
    return 0;
}
int deQueue(taskQueue_t *pTaskQueue){
    task_t *pCur = pTaskQueue->pFront;
    pTaskQueue->pFront = pTaskQueue->pFront->pNext;
    free(pCur);
    --pTaskQueue->size;
}