#include <func.h>

#define BUFFER_SIZE 4096
//接受客户端ls命令列出目录文件 文件名加空格
//高哥
int dirLs(char *specific, char *buf)
{
    bzero(buf, BUFFER_SIZE);

    //此处可调用dirPwd打印当前路径

    int ret = chidir(specific); //可填dirPwd返回值
    if (ret == -1)
    {
        strcpy(buf, "ERROR: No such directory, please check your input\n");
        return -1; //-1: ERROR: No such directory, please check your input\n
        //或许应该返回errno???
    }

    //文件不存在与打开路径失败剥离开
    DIR *dirp = opendir(specific);
    if (dirp == NULL)
    {
        strcpy(buf, "ERROR: failed to opendir\n");
        return -2; //-2:ERROR: failed to opendir
        //或许应该返回errno???
    }

    struct dirent *pdirent;

    while ((pdirent = readdir(dirp)))
    {
        //暂未处理ls的参数是文件的情况
        if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
        {
            continue;
        }

        strcat(buf, pdirent->d_name);
        strcat(buf, " ");
    }
    //读取完毕或报错都会走到这一行

    //如果最后一个是空格，删掉
    int len = strlen(buf);
    if (buf[len - 1] == ' ')
    {
        buf[len - 1] = 0;
        // len--;
    }
    return 0;
}

int main()
{

    return 0;
}