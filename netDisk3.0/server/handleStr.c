#include "headOfServer.h"
int commandAnalysis(char *command, char *specific)
{
    if (strlen(command) == 0)
    {
        return -1;
    }

    // char *commandmap[] = {"cd", "mkdir", "ls", "upload", "download", "rm", "pwd", "登录", "注册"};

    char *commandmap[] = {"cd", "mkdir", "ls", "upload", "download", "rm", "pwd", "用户名", "密文"};
    char *word = strtok(command, " ");
    char newCommand[10] = {0};

    strcpy(newCommand, word);
    // puts(newCommand);
    // for (int i = 0; i < 8; ++i)这里debug长达晚上到早上第一节课。。。。。。。。。i压根到不了8
    for (int i = 0; i < 9; ++i)
    {
        if (strcmp(newCommand, commandmap[i]) == 0)
        {
            // if (i == 7 || i == 8) //去客户端瞅了一眼，是直接发过来的汉字-----------------应该是发送端动了手脚，做了映射，这里能汉字匹配
            //{
            //     char usrname[10] = {0};
            //     char passwd[10] = {0}; //这里需要修改加盐------后面的在 fart ------划重点，密码长度在这里，加盐后密文可能会超长，如太长 handleCommand.c 可能也需要修改
            //     word = strtok(NULL, " ");
            //     strcpy(usrname, word);
            //     if (i == 8)
            //     {
            //         strcpy(specific, usrname);
            //         return i;
            //     }
            //     word = strtok(NULL, " ");
            //     strcpy(passwd, word);
            //     int ret = passwdVerify(usrname, passwd);
            //     if (ret == 0)
            //     {
            //         //登陆成功
            //         strcpy(specific, usrname);
            //         return i;
            //     }
            //     else
            //         return -1;
            // }
            if (i == 7) //客户端发的是用户名
            {
                // char usrname[10] = {0};
                word = strtok(NULL, " ");
                // strcpy(usrname, word);
                // 理解失误，specific为传入传出，把usrname从command中分离出来通过specific传出去，return 7 使 handleEvent 走到分支 7 ，使用 specific 赋值给用户名
                strcpy(specific, word);
            }
            else if (i == 8) //客户端发的是密文
            {
                word = strtok(NULL, " ");
                strcpy(specific, word);
            }
            else if (!(i == 2 || i == 6))
            {
                word = strtok(NULL, " ");
                strcpy(specific, word);
            }
            return i;
        }
    }
    return -1;
}

//划重点，高频使用
void printRes(int sockFd)
{
    int len = 0;
    char res[1024] = {0};
    int ret = recvn(sockFd, &len, sizeof(int));
    // ERROR_CHECK(ret,-1,"recv");
    ret = recvn(sockFd, res, len);
    // ERROR_CHECK(ret,-1,"recv");
    puts(res);
    return;
}