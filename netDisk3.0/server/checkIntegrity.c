#include "headOfServer.h"

/*
检测文件分为三种情况
文件存在:
    发送文件存在字符串的小火车
文件不完整:
    发送文件不完整字符串的小火车
    发送不完整文件大小的小火车
文件不存在:
    发送文件不存在的小火车
*/

int checkIntegrity(char *fileName, int netFd)
{
    char incFileName[20];
    strcpy(incFileName, fileName);
    strcat(incFileName, ".inc");
    puts(incFileName);
    char curPath[1024] = {0};
    getcwd(curPath, 1024);
    puts(curPath);
    DIR *dirp = opendir(curPath);
    if (dirp == NULL)
    {
        return -2;
    }
    struct dirent *pdirent;
    int i;
    for (i = 0; (pdirent = readdir(dirp)) && i < 50; i++)
    { // 写的什么玩意。。。。。最多检索 50 次，这是在搞毛线呢
        if (strcmp(pdirent->d_name, fileName) == 0)
        {
            //发送检测消息
            train_t t;
            t.trainLength = 10;
            memcpy(t.trainBody, "file exist", 10);
            send(netFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
            printf("file exist\n"); // cs-----------------------------------------------
            return 1;               //要传输的文件存在
        }
        else if (strcmp(pdirent->d_name, incFileName) == 0)
        {
            //发送检测消息
            train_t t;
            t.trainLength = 15;
            memcpy(t.trainBody, "file incomplete", 15);
            send(netFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
            //检测文件大小
            int fd = open(incFileName, O_RDONLY);
            struct stat statbuf;
            int ret = fstat(fd, &statbuf); //获取文件的总大小
            ERROR_CHECK(ret, -1, "fstat");
            t.trainLength = sizeof(statbuf.st_size); //把文件的大小装入小火车
            printf("已接收了%ld\nBytes", statbuf.st_size);
            memcpy(t.trainBody, &statbuf.st_size, sizeof(statbuf.st_size));
            send(netFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
            printf("file incomplete\n"); // cs-----------------------------------------------
            return 2;                    //要传输的文件不完整
        }
    }
    train_t t;
    t.trainLength = 14;
    memcpy(t.trainBody, "file not exist", 14);
    send(netFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
    printf("file not exist\n"); // cs-----------------------------------------------
    return 0;
}