
#include "headOfClient.h"
int fileUpLoad(int netFd,char *specific){
    char temp[100]="upload ";
    strcat(temp,specific);//specific是文件名
    puts(temp);
    train_t t;
    t.trainLength=strlen(temp);
    strcpy(t.trainBody,temp);
    send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//传的是upload file2
    //send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//传的是upload file2

    memset(&t,0,sizeof(t));//发送完文件名，清空小货车,这个文件名是用来让服务端进行switch

    //结构体保存文件信息
    int fd = open(specific, O_RDWR);
    ERROR_CHECK(fd,-1,"open");
    struct stat statbuf;    
    //发送文件名小火车
    t.trainLength=strlen(specific);
    printf("%d\n",t.trainLength);
    strcpy(t.trainBody,specific);

    puts(t.trainBody);
    int ret = send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文件名
    puts(t.trainBody);
    ERROR_CHECK(ret,-1,"send");

    t.trainLength = sizeof(statbuf.st_size);//把文件的大小装入小火车

    memcpy(t.trainBody,&statbuf.st_size,sizeof(statbuf.st_size));
    send(netFd,&t,sizeof(t.trainLength)+t.trainLength,MSG_NOSIGNAL);
    //发送完文件名会接收到接收端发来的检测文件信息
    int dataLength;
    char msgFromCheck[1024] = {};
    recvn(netFd, &dataLength, sizeof(int));
    recvn(netFd, msgFromCheck, dataLength);
    puts(msgFromCheck);
    int fileLength;
    printf("-------%d\n", dataLength);
    if(dataLength == 10){
        //要传输的文件存在
        return 0;
    }
    else if(dataLength == 15){
        //要传输的文件不完整
        int dataLength2;
        char incLength[1024] = {};
        recvn(netFd, &dataLength2, sizeof(int));
        recvn(netFd, incLength, dataLength2);
        fileLength = atoi(incLength);
        printf("--------------------fileLength--------------%d\n", fileLength);
    }
    else{
        //要传输的文件不存在
        fileLength = 0;
    }

    //检测需要传输多大的文件
    ret = fstat(fd,&statbuf);//获取文件的总大小
    ERROR_CHECK(ret,-1,"fstat");
    if(statbuf.st_size - fileLength >= 102400){
        //需要传输的文件超过100M，采用mmap传输
        char* p = (char* )mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        off_t total = (off_t)fileLength;
        while(total < statbuf.st_size){
            if(statbuf.st_size - total > sizeof(t.trainBody)){
                t.trainLength = sizeof(t.trainBody);
            }
            else{
                //最后一次传输
                t.trainLength = statbuf.st_size - total;
            }
            memcpy(t.trainBody, p + total, t.trainLength);
            total += (off_t)t.trainLength;
            int ret = send(netFd, &t, sizeof(int) + t.trainLength, MSG_NOSIGNAL);
            if(ret == -1){
                perror("send");
                break;
            }
        }
        t.trainLength = 0;
        send(netFd, &t, 4, MSG_NOSIGNAL);
        munmap(p, statbuf.st_size);
    }
    else{
        //需要传输的文件不大，采用原始方法传输
        int i=0;
        //开始传输文件体，首先将read函数读指针调至n处
        read(fd, NULL, fileLength);
        //开始正常读取发送
        while(1){
            bzero(&t,sizeof(t));
            ret = read(fd,t.trainBody,sizeof(t.trainBody));
            ERROR_CHECK(ret,-1,"read");
            if(ret != 1024){
                printf("ret = %d\n", ret);
                break;
            }
            if(i==1) break;
            if(ret == 0){
                break;
            }
            t.trainLength = ret;
            send(netFd,&t,sizeof(t.trainLength)+t.trainLength,MSG_NOSIGNAL);
            //发送文件内容
        }
        t.trainLength = 0;
        send(netFd,&t,sizeof(t.trainLength),0);
    }
    //发送终止
    close(fd);
    return 0;
}