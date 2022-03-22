#include <func.h>

int main(int argc,char*argv[])
{
    char passwd[] = "1234";
    char salt[] = "402KQA0c8";
    printf("%s\n", crypt(passwd, salt));
    return 0;
}

