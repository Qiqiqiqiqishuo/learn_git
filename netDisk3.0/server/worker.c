//当前小火车处理EOF段错误!!!!!
#include <func.h>
#include "headOfServer.h"
#include "threadPool.h"
#include "taskQueue.h"
#include "manipulate_mysql.h"

int makeWorker(threadPool_t *pThreadPool)
{
    for (int i = 0; i < pThreadPool->threadNum; ++i)
    {
        pthread_create(pThreadPool->tidArr + i, NULL, threadFunc, (void *)pThreadPool);
    }
}
void unlock(void *parg)
{
    pthread_mutex_t *pMutex = (pthread_mutex_t *)parg;
    pthread_mutex_unlock(pMutex);
}
void *threadFunc(void *parg)
{
    threadPool_t *pThreadPool = (threadPool_t *)parg;
    while (1)
    {
        int netFd;
        pthread_mutex_lock(&pThreadPool->mutex);
        pthread_cleanup_push(unlock, &pThreadPool->mutex);
        while (pThreadPool->taskQueue.size == 0 && pThreadPool->isThreadPoolRunning == 1)
        {
            pthread_cond_wait(&pThreadPool->cond, &pThreadPool->mutex);
        }
        if (pThreadPool->isThreadPoolRunning == 0)
        {
            puts("child quit");
            pthread_exit(NULL); //不能用return，受到cleanup的影响
        }
        puts("GetTask");
        netFd = pThreadPool->taskQueue.pFront->netFd;
        // int sockFd =pThreadPool->sockFd;
        deQueue(&pThreadPool->taskQueue);
        pthread_cleanup_pop(1);
        handleEvent(netFd);
        printf("thread done!, tid = %lu\n", pthread_self());
    }
}

int handleEvent(int netFd)
{
    // char pwd[100]="/home/mliuxb/netDisk/threadPool/usr";
    char pwd[100] = "/"; // pwd除为根目录的情况下一般不以'/'结尾
    // char pwdFake[100]="/";
    char temp[100] = {0};
    char usrname[100] = {0};
    char ciphertext[256] = {0};

    while (1)
    {
        int len = 0;
        char command[256] = {0}; // 100->256，因密文可能就100位了
        memset(command, 0, sizeof(command));
        bzero(command, sizeof(command));
        puts(command); //会打印一个换行
        // memset(command,0,sizeof(command));
        int ret = recvn(netFd, &len, sizeof(int));
        ERROR_CHECK(ret, -1, "recv");
        ret = recvn(netFd, command, len);
        ERROR_CHECK(ret, -1, "recv");
        puts(command); //当前小火车处理EOF段错误!!!!!

        printf("command : %s\n", command);

        char specific[256] = {0}; // 100->256，因密文可能就100位了，这里多放一点点
        int commandId = commandAnalysis(command, specific);
        printf("commandId=%d\n", commandId);
        puts(specific);

        switch (commandId)
        {
        case 0: //改变路径
            dirCd(specific, pwd, usrname);
            printf("commandId=%d\n", commandId);
            break;
        case 1: //创建目录
            bzero(temp, sizeof(temp));
            sprintf(temp, "%s/%s", pwd, specific);
            // puts(temp);
            dirMake(temp);
            printf("commandId=%d\n", commandId);
            break;
        case 2: // cd,ls,mkdir类
            puts(pwd);
            puts("zheli");
            dirLs(netFd, pwd);
            printf("commandId=%d\n", commandId);
            break;
        case 3: //上传文件
            fileUpLoad(netFd);
            printf("%d\n", commandId);
            break;
        case 4: //下载文件
            fileDownLoad(netFd, specific);
            printf("commandId=%d\n", commandId);
            break;
        case 5: //删除文件
            fileRemove(specific);
            printf("commandId=%d\n", commandId);
            break;
        case 6: //获取当前工作目录
            dirPwd(netFd, pwd);
            printf("commandId=%d\n", commandId);
            break;
        case 7: //通过收到的用户名处理登录（查salt）/注册（随机生成salt并存数据库）行为，向客户端返回 salt
            // login(pwd, specific, usrname);
            strcpy(usrname, specific); // commandAnalysis 函数将 specific 置为了用户名， 这里 将其拷贝给 usrname
            int retrieve_send_salt_ret = retrieve_send_salt_by_name(usrname, netFd);
            printf("成功commandId=%d\n", commandId);
            break;
        case 8: //比对客户端发送过来的密文，进行判断
            // regist(specific);
            strcpy(ciphertext, specific);
            int retrieve_check_ciphertext_by_name_ret = retrieve_check_ciphertext_by_name(usrname, ciphertext, netFd);
            printf("成功commandId = %d \n", commandId);
            break;
        default:
            break; //命令不对头无视之，小火车EOF翻车
        }
    }
    close(netFd);
    return 0;
}