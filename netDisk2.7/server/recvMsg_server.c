#include<func.h>
#include "headOfServer.h"
int recvn(int netFd,void *p, int n){
    int total = 0;
    int ret;
    char *pTmp = (char *)p;
    while(total < n){
        ret = recv(netFd,pTmp+total,n-total,0);
        ERROR_CHECK(ret,-1,"recv");
        total+= ret;
    }
    return 0;
}

int recvFile(int netFd){
    int dataLength;
    char name[1024] = {0};
    int ret =recvn(netFd,&dataLength,sizeof(int));
    ERROR_CHECK(ret,-1,"recv");
    ret = recvn(netFd,name, dataLength);
    
    //接受完文件名小火车，开始检测文件，并为发送方提供信息
    ret = checkIntegrity(name, netFd);
    if(ret == 1) return 0;
    printf("%d\n",dataLength);
    puts(name);
    ERROR_CHECK(ret,-1,"recv");
    char transFileName[1024] = {};
    strcpy(transFileName, name);
    strcat(transFileName, ".inc");
    int fd = open(transFileName, O_RDWR|O_CREAT, 0666);
    ERROR_CHECK(fd,-1,"open");
    char buf[1024] = {0};
    long fileSize;
    recvn(netFd,&dataLength,sizeof(int));//先收火车头
    recvn(netFd,&fileSize,sizeof(fileSize));//再收文件长度
    printf("fileSize = %ld\n", fileSize);
    ftruncate(fd,fileSize);
    off_t downSize = 0;
    off_t lastSize = 0;
    off_t slice = fileSize/1000;
    struct timeval begTime,endTime;
    gettimeofday(&begTime,NULL);
    while(1){
        ret = recvn(netFd,&dataLength,sizeof(int));
        ERROR_CHECK(ret,-1,"recv");
        if(dataLength==0){
            break;
        }
        if(dataLength != 1024){
            printf("dataLength = %d\n", dataLength);
        }
        ret = recvn(netFd,buf,dataLength);
        ERROR_CHECK(ret,-1,"recv");
        downSize += dataLength;
        if(downSize-lastSize > slice){
            printf("%.2lf%s\r", 100.0*downSize/fileSize,"%");
            fflush(stdout);
            lastSize = downSize;
        }
        write(fd,buf,dataLength);
    }
    rename(transFileName, name);
    printf("100.00%s\n", "%");
    gettimeofday(&endTime,NULL);
    printf("total time = %ld\n", 1000000*(endTime.tv_sec-begTime.tv_sec) + endTime.tv_usec-begTime.tv_usec);
}