#include <func.h>
#include "headOfServer.h"
int fileDownLoad(int netFd,const char *specific){
    train_t t={4,"file"};
    send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文发送数据类型
    memset(&t,0,sizeof(t));
    
    t.trainLength=strlen(specific);
    strcpy(t.trainBody,specific);
    printf("filename:%s\n",specific);
    int fd = open(specific, O_RDWR);
    ERROR_CHECK(fd,-1,"open");
    //printf("%d\n",fd);
    int ret = send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文件名
    ERROR_CHECK(ret,-1,"send");

    struct stat statbuf;
    ret = fstat(fd,&statbuf);//获取文件的总大小
    ERROR_CHECK(ret,-1,"fstat");
    t.trainLength = sizeof(statbuf.st_size);//把文件的大小装入小火车

    memcpy(t.trainBody,&statbuf.st_size,sizeof(statbuf.st_size));
    send(netFd,&t,sizeof(t.trainLength)+t.trainLength,MSG_NOSIGNAL);
    while(1){
        bzero(&t,sizeof(t));
        ret = read(fd,t.trainBody,sizeof(t.trainBody));
        ERROR_CHECK(ret,-1,"read");
        if(ret != 1024){
            printf("ret = %d\n", ret);
        }
        if(ret == 0){
            break;
        }
        t.trainLength = ret;
        send(netFd,&t,sizeof(t.trainLength)+t.trainLength,MSG_NOSIGNAL);
        //发送文件内容
    }
    
    t.trainLength = 0;
    send(netFd,&t,sizeof(t.trainLength),0);
    //发送终止
    close(fd);
}