#include <func.h>
#define BUFFER_SIZE 4096

//接收客户端列出文件路径的命令，起始路径应为server根目录
//高哥
// int dirPwd(char *specific, char *buf)
int dirPwd(char *buf)
{
    ////为啥会有specific参数,假定为正在浏览的路径
    // bzero(buf, BUFFER_SIZE);

    // int ret = chdir(specific);
    // if (ret == -1)
    //{
    //     strcpy(buf, "ERROR: No such file, please check your input\n");
    //     return -1; //-1: ERROR: No such file, please check your input
    //     //或许应该返回errno???
    // }

    char *pret = getcwd(buf, BUFFER_SIZE);
    if (pret == NULL)
    {
        // strcpy(buf, "ERROR: failed to get absolute path");
        return -2; //-2: ERROR: failed to get absolute path
        //或许应该返回errno???
    }
    else
    {
        //处理起始路径
        // 当前处理方法如服务端根目录改变则需要维护
        // buffer = buffer + 偏移量;仅示意，不是这样写
        // memmove(buffer, buffer+偏移量, BUFFER_SIZE-偏移量);
        return 0;
    }
}

int main()
{
    // char specific[] = ".";
    char buf[BUFFER_SIZE] = {0};
    int ret = dirPwd(buf);
    // int ret = chdir(".");
    // ERROR_CHECK(ret, -1, "chdir");
    printf("%s-end\n", buf);
    return 0;
}