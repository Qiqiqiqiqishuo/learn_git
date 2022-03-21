#include "headOfServer.h"
int commandAnalysis(char *command,char *specific){
    if(strlen(command)==0) return -1;
    char *commandmap[]={"cd","mkdir","ls","upload","download","rm","pwd","登录","注册"};
    char *word=strtok(command," ");
    char newCommand[10]={0};

    strcpy(newCommand,word);
    //puts(newCommand);
    for(int i=0;i<8;++i){
        if(strcmp(newCommand,commandmap[i])==0){
            if(i==7||i==8){
                char usrname[10]={0};
                char passwd[10]={0};
                word=strtok(NULL," ");
                strcpy(usrname,word);
                if(i==8) {
                    strcpy(specific,usrname);
                    return i;
                }
                word=strtok(NULL," ");
                strcpy(passwd,word);
                int ret=passwdVerify(usrname,passwd);
                if(ret==0){
                    //登陆成功
                    strcpy(specific,usrname);
                    return i;
                }else return -1;
            }else if(!(i==2||i==6)){
                word=strtok(NULL," ");
                strcpy(specific,word);
            }
            return i;
        }
    }
    return -1;
}

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