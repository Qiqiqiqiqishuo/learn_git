// passwd = getpass("密码："); //划重点，自己没学过
#include <func.h>
#include "headOfClient.h"

int main(int argc, char *argv[])
{
    //./client 192.168.119.128 1234
    printf("用户名：");
    char usrname[10] = {0};
    scanf("%s", usrname);
    //输入用户名密码
    char *passwd;
    passwd = getpass("密码："); //划重点，没学过

    ARGS_CHECK(argc, 3);
    // int retPasswdVerify=passwdVerify();
    // ERROR_CHECK(retPasswdVerify,-1,"passwdVerify");
    // if(retPasswdVerify==0)  break;

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sockFd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "connect");
    int epfd = epoll_create(10);
    epollAdd(sockFd, epfd);
    epollAdd(STDIN_FILENO, epfd);
    struct epoll_event readyList[2];
    char buf[1024] = {0};
    /*//发送注册信息
    train_t t;
    memset(&t,0,sizeof(t));
    sprintf(buf,"%s %s %s","登录",usrname,passwd);
    t.trainLength=strlen(buf);
    printf("%d\n",t.trainLength);
    puts(buf);
    strcpy(t.trainBody,buf);
    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
    */
    //    //发送登录信息
    //    train_t t;
    //    memset(&t,0,sizeof(t));
    //    sprintf(buf,"%s %s %s","登录",usrname,passwd);
    //    t.trainLength=strlen(buf);
    //    printf("%d\n",t.trainLength);
    //    puts(buf);
    //    strcpy(t.trainBody,buf);
    //    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
    //

    //  登陆完成前乱瞎搞（乱输东西）可能会导致意想不到的后果！！！
    //实现客户“无感注册登录”，“提高用户体验”(减少登录失败的概率，输错用户名就自动创建新账号)的同时获取更多注册客户数量
    //客户端无论是登录还是注册操作逻辑完全相同
    //
    //  C <----> S
    //  1. 发送用户名
    //  2. 接受盐值
    //  3. crypt后发送密文
    //
    //  Client <----> Customer
    //  1. 输入用户名
    //  2. 输入密码
    //
    //  从而提高用户注册数量(除正确输入已注册用户名外每次都会注册一个用户)
    //  登陆完成前乱瞎搞（乱输东西）可能会导致意想不到的后果！！！

    //发送用户名
    train_t t;
    memset(&t, 0, sizeof(t));
    bzero(buf, sizeof(buf)); // 1024
    sprintf(buf, "%s %s", "用户名", usrname);
    t.trainLength = strlen(buf);
    printf("%d\n", t.trainLength);
    puts(buf); //检视要发送的内容
    strcpy(t.trainBody, buf);
    send(sockFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

    while (1)
    {
        int readyNum = epoll_wait(epfd, readyList, 2, -1);
        for (int i = 0; i < readyNum; ++i)
        {
            if (readyList[i].data.fd == STDIN_FILENO)
            {
                bzero(buf, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                char specific[100] = {0};
                if (commandAnalysis(buf, specific) == 3)
                {
                    char new_specific[100] = {0};
                    strncpy(new_specific, specific, strlen(specific) - 1);
                    fileUpLoad(sockFd, new_specific);
                }
                else
                {
                    train_t t;
                    bzero(&t, sizeof(t));
                    t.trainLength = strlen(buf);
                    // printf("len=%d\n",t.trainLength);
                    strncpy(t.trainBody, buf, t.trainLength - 1);
                    // puts(t.trainBody);
                    send(sockFd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
                }
            }
            else
            {
                char msgType[1024] = {0};
                getMsgType(sockFd, msgType);

                if (strcmp(msgType, "string") == 0)
                {

                    printRes(sockFd);
                }
                else if (strcmp(msgType, "salt") == 0)
                { //接收salt并加盐向服务端发送密文
                }
                else
                    recvFile(sockFd);
            }
        }
    }
    close(sockFd);
}