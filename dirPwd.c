#include <func.h>
//接收客户端列出文件路径的命令，起始路径应为server根目录
//高哥
#define BUFFER_SIZE 4096
int dirPwd(char *specific, char *buf)
{
    bzero(buf, BUFFER_SIZE);

    int ret = chdir(specific);
    if (ret == -1)
    {
        strcpy(buf, "ERROR: No such file, please check your input\n");
        return -1; //-1: ERROR: No such file, please check your input
        //或许应该返回errno???
    }

    char *pret = getcwd(buf, BUFFER_SIZE);
    if (pret == NULL)
    {
        strcpy(buf, "ERROR: failed to get absolute path");
        return -2; //-2: ERROR: failed to get absolute path
        //或许应该返回errno???
    }
    else
    {
        //处理起始路径
        // buffer = buffer + 偏移量;
        return 0;
    }
}

int main()
{

    return 0;
}