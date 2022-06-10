// EOF-->段错误
// memmove-->不拷贝
#include <func.h>
#include "threadPool.h"
#include "headOfServer.h"
int exitPipe[2];
void sigFunc(int signum)
{
    printf("signum is comming!\n");
    write(exitPipe[1], "1", 1);
}
int main(int argc, char *argv[])
{
    //./server 10.0.24.5 5783 4
    ARGS_CHECK(argc, 4);
    pipe(exitPipe);
    if (fork() != 0)
    {
        //父进程
        close(exitPipe[0]); //关闭读端
        signal(SIGUSR1, sigFunc);
        wait(NULL);
        exit(0);
    }
    close(exitPipe[1]); //子进程关闭写端
    mkdir("./usr", 0777);
    // chdir("/home/mliuxb/netDisk/threadPool/usr");
    chdir("./usr");
    int workerNum = atoi(argv[3]);
    threadPool_t threadPool;
    threadPoolInit(&threadPool, workerNum);
    int sockFd;
    makeWorker(&threadPool);
    tcpInit(argv[1], argv[2], &sockFd);
    threadPool.sockFd = sockFd;
    int epfd = epoll_create(10);
    epollAdd(sockFd, epfd);
    epollAdd(exitPipe[0], epfd);
    struct epoll_event readyList[2];
    while (1)
    {
        int readyNum = epoll_wait(epfd, readyList, 2, -1);
        puts("epoll ready");
        for (int i = 0; i < readyNum; ++i)
        {
            if (readyList[i].data.fd == sockFd)
            {
                int netFd = accept(sockFd, NULL, NULL);
                pthread_mutex_lock(&threadPool.mutex);
                enQueue(&threadPool.taskQueue, netFd);
                puts("new Task");
                pthread_cond_signal(&threadPool.cond);
                pthread_mutex_unlock(&threadPool.mutex);
            }
            else if (readyList[i].data.fd == exitPipe[0])
            {
                // for(int j = 0; j < workerNum; ++j){
                //     pthread_cancel(threadPool.tidArr[j]);
                // }
                puts("I am going to die!");
                threadPool.isThreadPoolRunning = 0; //避免带锁死设置的标志位
                pthread_cond_broadcast(&threadPool.cond);
                for (int j = 0; j < workerNum; ++j)
                {
                    pthread_join(threadPool.tidArr[j], NULL);
                }
                puts("Exit");
                exit(0);
            }
        }
    }
}