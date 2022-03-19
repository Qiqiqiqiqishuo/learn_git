#include <func.h>
//#define BUFFER_SIZE 4096
//设计改为在函数内部发送，直接在函数内给buf分配充足，不用这玩意了

//接受客户端ls命令列出目录文件 文件名加空格
//高哥

// int dirLs(char *specific, char *buf);

int dirLs(int nfd, char *specific)
{
    //不应处理：不讲武德到参数不合法的情况(输错了输成两段等情况)暂不处理（存在将错就错的情况）
    //
    //不会处理：
    //      1. 参数的合理性：如果填入路径在server之外，则应报错
    //      这样想下去好像没有尽头了.....好复杂
    //
    //
    //
    //  ？？？是否要考虑到将错就错的情况，给个提示？
    //          -->第一行打印 即将打印的-->"{[目录/文件]的[绝对路径]}:"
    //      ↑暂定处理（注释掉了，还是先不处理了）
    //      ↓暂未处理
    //      参数为文件的情况暂未处理
    //      我目前能想到的处理方法是opendir然后遍历目录流，
    //      获取属性，判断输入的是路径还是文件(没有具体实现的思路)
    //
    //--------则specific应恰为一个绝对路径或相对路径
    //
    //应处理：
    //      1. 路径不存在-->print error
    //      2. 输入的是文件-->则打印该文件名

    // char buf[BUFFER_SIZE] = {0};
    //  bzero(buf, BUFFER_SIZE);

    char buf[4096] = {0};

    ////此处可调用dirPwd打印当前路径

    // int ret = chidir(specific); //可填dirPwd返回值
    // if (ret == -1)
    //{
    //     // strcpy(buf, "ERROR: No such directory, please check your input\n");
    //     return -1; //-1: ERROR: No such directory, please check your input\n
    //     //或许应该返回errno???
    // }

    //路径不存在与打开路径失败剥离开
    DIR *dirp = opendir(specific);
    if (dirp == NULL)
    {
        // strcpy(buf, "ERROR: failed to opendir, please check your input\n");
        return -2; //-2:ERROR: failed to opendir, please check your input
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
        strcat(buf, "  "); //两个空格
    }
    //读取完毕或报错都会走到这一行
    //错误处理待讨论

    //如果最后一个是空格，删掉
    int len = strlen(buf);
    while (buf[len - 1] == ' ')
    {
        buf[len - 1] = 0;
        len--;
    }
    // buf末尾没有换行，如有需要可添加

    int ret = send(nfd, buf, strlen(buf), 0);
    //错误处理待讨论

    return 0;
}

// int main()
//{
//     char specific[] = ".";
//     // char buf[BUFFER_SIZE] = {0};
//     // int ret = dirLs(specific, buf);
//     // printf("%s-end\n", buf);
//
//     int ret = dirLs(nfd, specific);
//     //错误处理待讨论
//
//     return 0;
// }