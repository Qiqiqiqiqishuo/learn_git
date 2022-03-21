#include "headOfClient.h"
int login(int sockFd){
    printf("用户名：");
    char usrname[10]={0};
    scanf("%s",usrname);
    //输入用户名密码
    char *passwd;
    passwd=getpass("密码：");
    char buf[100];
    //发送登录+用户名
    train_t t;
    memset(&t,0,sizeof(t));
    sprintf(buf,"%s %s","登录",usrname);
    t.trainLength=strlen(buf);
    //printf("%d\n",t.trainLength);
    //puts(buf);
    strcpy(t.trainBody,buf);
    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送 "登录+usrname"
    
    //接收盐

    int dataLength;
    char salt[1024] = {0};
    int ret =recvn(sockFd,&dataLength,sizeof(int));
    ERROR_CHECK(ret,-1,"recv");
    ret = recvn(sockFd,salt, dataLength);
    printf("数据长度%d\n",dataLength);
    puts(salt);
    if(strcmp(salt,"用户未注册")==0)    return -1;
    //发送盐和密文的加密
    char *secret=crypt(passwd,salt);
    bzero(buf,sizeof(buf));
    strcpy(buf,secret);
    
    memset(&t,0,sizeof(t));
    t.trainLength=strlen(buf);
    printf("火车长度：%d\n",t.trainLength);
    puts(buf);
    strcpy(t.trainBody,buf);
    send(sockFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
    //接收登录是否成功消息
    char result[1024] = {0};
    ret =recvn(sockFd,&dataLength,sizeof(int));
    ERROR_CHECK(ret,-1,"recv");
    ret = recvn(sockFd,result, dataLength);
    printf("数据长度%d\n",dataLength);
    puts(result);
    if(strcmp(result,"密码正确,登陆成功"))
        return 0;
    return -1;
}