#include <func.h>
#include "headOfServer.h"
int recvn(int netFd, void *p, int n)
{
    int total = 0;
    int ret;
    char *pTmp = (char *)p;
    while (total < n)
    {
        ret = recv(netFd, pTmp + total, n - total, 0);
        ERROR_CHECK(ret, -1, "recv");
        total += ret;
    }
    return 0;
}

int recvFile(int netFd)
{
    int dataLength;
    char name[1024] = {0};
    int ret = recvn(netFd, &dataLength, sizeof(int));
    ERROR_CHECK(ret, -1, "recv");
    ret = recvn(netFd, name, dataLength);

    //接受完文件名小火车，开始检测文件，并为发送方提供信息
    ret = checkIntegrity(name, netFd);
    if (ret == 1)
        return 0;
    printf("%d\n", dataLength); // 服务端提示
    puts(name);                 // 服务端提示
    ERROR_CHECK(ret, -1, "recv");
    char transFileName[1024] = {};
    strcpy(transFileName, name);
    strcat(transFileName, ".inc");
    // int fd = open(transFileName, O_RDWR | O_CREAT, 0666);
    int fd = open(transFileName, O_RDWR | O_APPEND | O_CREAT, 0666);
    ERROR_CHECK(fd, -1, "open");
    char buf[1024] = {0};
    long fileSize;
    // long fileSize = 0;
    recvn(netFd, &dataLength, sizeof(int)); //先收火车头，等于没写，后面也没用
    // recvn(netFd, &fileSize, sizeof(fileSize)); //再收文件长度（火车内容），应该 填  dataLength ，但这个碰巧一样，workable。。。。。就搁这吧
    recvn(netFd, &fileSize, dataLength); //再收文件长度（火车内容），应该 填  dataLength ，但这个碰巧一样，workable。。。。。就搁这吧
    printf("fileSize = %ld\n", fileSize);
    // ftruncate(fd, fileSize); // 这里不应该 ftruncate 的 啊，。，。，。离谱，这是子啊搞毛线

    // 合着每次都是从头开始写的。。。。麻了
    // off_t downSize = 0;
    // off_t lastSize = 0;
    struct stat statBuf;
    fstat(fd, &statBuf);
    off_t downSize = statBuf.st_size;
    off_t lastSize = downSize;
    off_t slice = fileSize / 1000;
    struct timeval begTime, endTime;
    gettimeofday(&begTime, NULL);
    while (1)
    {
        ret = recvn(netFd, &dataLength, sizeof(int));
        ERROR_CHECK(ret, -1, "recv");
        if (dataLength == 0)
        {
            break;
        }
        if (dataLength != 1024)
        {
            printf("dataLength = %d\n", dataLength);
        }
        ret = recvn(netFd, buf, dataLength);
        ERROR_CHECK(ret, -1, "recv");
        downSize += dataLength;
        if (downSize - lastSize > slice)
        {
            // printf("%.2lf%s\r", 100.0 * downSize / fileSize, "%");
            printf("%.2lf%s\r", 100.0 * downSize / fileSize, "%");
            fflush(stdout);
            lastSize = downSize;
        }
        write(fd, buf, dataLength);
    }
    rename(transFileName, name);
    printf("100.00%s\n", "%");
    gettimeofday(&endTime, NULL);
    printf("total time = %ld\n", 1000000 * (endTime.tv_sec - begTime.tv_sec) + endTime.tv_usec - begTime.tv_usec);
}