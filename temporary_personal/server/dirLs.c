//接受客户端ls命令列出目录文件 文件名加空格
//高哥
int dirLs(char *specific, char *buf);

int main()
{
    char specific;
}
int dirLs(char *specific, char *buf)
{
    //不应处理：不讲武德到参数不合法的情况(输错了输成两段等情况)暂不处理（存在将错就错的情况）
    //  ？？？是否要考虑到将错就错的情况-->第一行打印 即将打印的-->"{[目录/文件]的[绝对路径]}:"
    //      ↑暂定处理
    //--------则specific应恰为一个绝对路径或相对路径
    //
    //应处理：
    //      1. 路径不存在-->perror
    //      2. 输入的是文件-->则打印该文件名
}