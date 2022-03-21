#include <func.h>
#include "headOfClient.h"

int main(int argc, char *argv[]){
    //./client 192.168.135.128 1234
    printf("用户名：");
    char usrname[10]={0};
    scanf("%s",usrname);
    //输入用户名密码
    char *passwd;
    passwd=getpass("密码：");

    ARGS_CHECK(argc,3);
        //int retPasswdVerify=passwdVerify();
        //ERROR_CHECK(retPasswdVerify,-1,"passwdVerify");
        //if(retPasswdVerify==0)  break;

    int sockFd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sockFd,(struct sockaddr*)&addr,sizeof(addr));
    ERROR_CHECK(ret,-1,"connect");
    int epfd = epoll_create(10);
    epollAdd(sockFd,epfd);
    epollAdd(STDIN_FILENO,epfd);
    struct epoll_event readyList[2];
    char buf[1024]={0};
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
    //发送登录信息
    train_t t;
    memset(&t,0,sizeof(t));
    sprintf(buf,"%s %s %s","登录",usrname,passwd);
    t.trainLength=strlen(buf);
    printf("%d\n",t.trainLength);
    puts(buf);
    strcpy(t.trainBody,buf);
    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);

    while(1){
        int readyNum = epoll_wait(epfd,readyList,2,-1);
        for(int i = 0; i < readyNum; ++i){
            if(readyList[i].data.fd == STDIN_FILENO){
                bzero(buf,sizeof(buf));
                read(STDIN_FILENO,buf,sizeof(buf));
                char specific[100]={0};
                if(commandAnalysis(buf,specific)==3){
                    char new_specific[100]={0};
                    strncpy(new_specific,specific,strlen(specific)-1);
                    fileUpLoad(sockFd,new_specific);
                }
                else {
                    train_t t;
                    bzero(&t,sizeof(t));
                    t.trainLength=strlen(buf);
                    //printf("len=%d\n",t.trainLength);
                    strncpy(t.trainBody,buf,t.trainLength-1);
                    //puts(t.trainBody);
                    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
                }
            }else{
                char msgType[1024]={0};
                getMsgType(sockFd,msgType);
                
                if(strcmp(msgType,"string")==0){
                    
                    printRes(sockFd);
                }
                else
                    recvFile(sockFd);
            }
        }
    }
    close(sockFd);
}