#include<func.h>
#include "headOfClient.h"
void printRes(int sockFd){
    int len=0;
    char res[1024] = {0};
    int ret =recvn(sockFd,&len,sizeof(int));
    //ERROR_CHECK(ret,-1,"recv");
    ret = recvn(sockFd,res, len);
    //ERROR_CHECK(ret,-1,"recv");
    puts(res);
    return;
}

void getMsgType(int sockFd,char *new_msgType){
    int dataLength=0;
    char msgType[1024] = {0};
    int ret =recvn(sockFd,&dataLength,sizeof(int));
    //ERROR_CHECK(ret,-1,"recv");
    //printf("%d\n",dataLength);
    ret = recvn(sockFd,msgType, dataLength);
    strcpy(new_msgType,msgType);
    //ERROR_CHECK(ret,-1,"recv");
    return ;
}
int commandAnalysis(const char *command,char *specific){
    char *commandmap[]={"cd","mkdir","ls","upload","download","remove","pwd"};
    char fresh_command[100]={0};
    strcpy(fresh_command,command);
    char *word=strtok(fresh_command," ");
    char newCommand[10]={0};

    strcpy(newCommand,word);
    for(int i=0;i<7;++i){
        if(strcmp(newCommand,commandmap[i])==0){
            if(!(i==2||i==6)){
                word=strtok(NULL," ");
                strcpy(specific,word);
            }
            return i;
        }
    }
    return -1;
}