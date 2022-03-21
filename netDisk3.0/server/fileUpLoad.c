#include<func.h>
#include "headOfServer.h"
int fileUpLoad(int netFd){//接受从客户端传来的数据
    recvFile(netFd);
}