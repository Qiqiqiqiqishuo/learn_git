#ifndef __TASKQUEUE__
#define __TASKQUEUE__
typedef struct task_s{
    int netFd;
    struct task_s *pNext;
} task_t;
typedef struct taskQueue_s{
    task_t * pFront;
    task_t * pRear;
    int size;
} taskQueue_t;
int enQueue(taskQueue_t *pTaskQueue, int netFd);
int deQueue(taskQueue_t *pTaskQueue);
#endif