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
int fileUpLoad(int netFd);
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
void login(char *pwd,char *specific,char *usrname);
int checkIntegrity(char* fileName, int netFd);


#include<syslog.h>
//一般信息记录，传入字符串，自动生成时间。
//具体要记录哪些信息，全部放在msg字符串中
//LOG_CONS用于在写入日志错误时，将错误信息写入系统控制台，一般不会发生错误不用管
//LOG_PID用于在消息上附加调用者的进程ID
#define INFO_LOG(msg){openlog("server",LOG_CONS|LOG_PID);syslog(LOG_INFO,"%s",msg);closelog();}
//效果：可在 /var/log/syslog中查看
//Mar 19 15:20:19 用户名 server[PID]: %s

//错误记录，传入整数(如-1或者错误的宏)，只用于设置了perror的错误，将错误的数字传入，可记录perror
#define ERROR_LOG(errno){openlog("server",LOG_CONS|LOG_PID);syslog(LOG_ERR,"%s",strerror(errno));closelog();}