#include<func.h>
#include "headOfServer.h"
int fileUpLoad(int netFd,char *pwd){//接受从客户端传来的数据
    char curPwd[100]={0};
    char curPwd_copy[100]={0};
    getcwd(curPwd,sizeof(curPwd));
    strcpy(curPwd_copy,curPwd);
    strcat(curPwd,pwd);
    chdir(curPwd);
    recvFile(netFd);
    chdir(curPwd_copy);
}