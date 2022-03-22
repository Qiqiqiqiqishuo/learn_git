#include <func.h>
int tcpInit(char *ip, char *port, int *pSockFd);
int epollAdd(int fd, int epfd);
int epollDel(int fd, int epfd);

typedef struct train_s
{
    int trainLength;
    char trainBody[1024]; //火车货箱最大容量1024字节
} train_t;

int fileUpLoad(int netFd, char *specific);
int commandAnalysis(const char *command, char *specific);
void getMsgType(int sockFd, char *new_msgType);
void printRes(int sockFd);
int recvn(int netFd, void *p, int n);
int recvFile(int netFd);
int passwdVerify();