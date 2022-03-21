#include "threadPool.h"
int tcpInit(char *ip, char *port, int *pSockFd);
int epollAdd(int fd, int epfd);
int epollDel(int fd, int epfd);
void *threadFunc(void *parg);
int makeWorker(threadPool_t *pThreadPool);
typedef struct train_s{
    int trainLength;
    char trainBody[1024];
}train_t;
int fileDownLoad(int netFd,const char *specific);
int fileUpLoad(int netFd,char *pwd);
int handleEvent(int netFd);
int recvn(int netFd,void *p, int n);
int recvFile(int netFd);
int dirCd(char *specific,char *pwd,char *usrname);
int dirLs(int nfd,char *pwd);
int dirPwd(int nfd,char *pwd);
int dirMake(const char* specific);
int fileRemove(const char* specific);
int commandAnalysis(char *command,char *specific);
int passwdVerify(const char* usrname,const char* passwd);
void login(int netFd,char *pwd,char *specific,int *loginState);
int checkIntegrity(char* fileName, int netFd);