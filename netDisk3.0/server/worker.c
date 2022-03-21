#include<func.h>
#include "headOfServer.h"
#include "threadPool.h"
#include "taskQueue.h"
int makeWorker(threadPool_t *pThreadPool){
    for(int i = 0; i < pThreadPool->threadNum; ++i){
        pthread_create(pThreadPool->tidArr+i,NULL,threadFunc,(void *)pThreadPool);
    }
}
void unlock(void *parg){
    pthread_mutex_t *pMutex = (pthread_mutex_t *)parg;
    pthread_mutex_unlock(pMutex);
}
void *threadFunc(void *parg){
    threadPool_t *pThreadPool = (threadPool_t *)parg;
    while(1){
        int netFd;
        pthread_mutex_lock(&pThreadPool->mutex);
        pthread_cleanup_push(unlock,&pThreadPool->mutex);
        while(pThreadPool->taskQueue.size == 0 && pThreadPool->isThreadPoolRunning == 1){
            pthread_cond_wait(&pThreadPool->cond,&pThreadPool->mutex);
        }
        if(pThreadPool->isThreadPoolRunning == 0){
            puts("child quit");
            pthread_exit(NULL);//不能用return，受到cleanup的影响
        }
        puts("GetTask");
        netFd = pThreadPool->taskQueue.pFront->netFd;
        //int sockFd =pThreadPool->sockFd;
        deQueue(&pThreadPool->taskQueue);
        pthread_cleanup_pop(1);
        handleEvent(netFd);
        printf("thread done!, tid = %lu\n", pthread_self());
    }
}

int handleEvent(int netFd){
    //char pwd[100]="/home/mliuxb/netDisk/threadPool/usr";
    char pwd[100]="/";
    //char pwdFake[100]="/";
    char temp[100]={0};
    char usrname[100]={0};

    while(1){
        int len=0;
        char command[100] = {0};
        memset(command,0,sizeof(command));
        bzero(command,sizeof(command));
        puts(command);
        //memset(command,0,sizeof(command));
        int ret =recvn(netFd,&len,sizeof(int));
        ERROR_CHECK(ret,-1,"recv");
        ret = recvn(netFd,command, len);
        ERROR_CHECK(ret,-1,"recv");
        puts(command);

        printf("command : %s\n",command);

        char specific[100]={0};
        int commandId=commandAnalysis(command,specific);
        printf("commandId=%d\n",commandId);
        puts(specific);
        
        switch (commandId)
        {
            case 0://改变路径
                dirCd(specific,pwd,usrname);
                printf("commandId=%d\n",commandId);
                break;
            case 1://创建目录
                bzero(temp,sizeof(temp));
                sprintf(temp,"%s/%s",pwd,specific);
                //puts(temp);
                dirMake(temp);
                printf("commandId=%d\n",commandId);
                break;
            case 2://cd,ls,mkdir类
                puts(pwd);
                puts("zheli");
                dirLs(netFd,pwd); 
                printf("commandId=%d\n",commandId); 
                break;
            case 3://上传文件
                fileUpLoad(netFd);
                printf("%d\n",commandId);
                break;
            case 4://下载文件
                fileDownLoad(netFd,specific);
                printf("commandId=%d\n",commandId);
                break;
            case 5://删除文件
                fileRemove(specific);
                printf("commandId=%d\n",commandId);
                break;
            case 6://获取当前工作目录
                dirPwd(netFd,pwd);
                printf("commandId=%d\n",commandId);
                break;
            case 7:
                
                login(pwd,specific,usrname);
                printf("成功commandId=%d\n",commandId);
                break;
            case 8:
                //regist(specific);
                break;
            default:
                break;
        }
    }
    close(netFd);
    return 0;
}