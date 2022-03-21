/*#include<func.h>

void help(){
    printf("用户第一个密码验证程序\n第一个参数为用户名！\n");
    exit(-1);
}

void error_quit(char *msg){
    perror(msg);
    exit(-2);
}

void get_salt(char *salt,char *password){
    int i,j;
    //取出salt,i记录密码字符下标，j记录$出现的次数
    for(int i=0,j=0;password[i]&&j!=3;++i){
        if(password[i]=='$')
            ++j;
    }
    strncpy(salt,password,i-1);
}

int main(int argc,char *argv[]){
    struct spwd *sp;
    char *password;
    char salt[512]={0};
    if(argc!=2)
        help();
    //输入用户名密码
    password=getpass("请输入密码：");
    //得到用户名以及密码，命令行参数的第一个参数为用户名
    if((sp=getspnam(argv[1]))==NULL)
        error_quit("获取用户名和密码");
    //得到salt，用得到的密码做参数
    get_salt(salt,sp->sp_pwdp);
    //进行密码验证
    if(strcmp(sp->sp_pwdp,crypt(password,salt))==0)
        printf("验证通过!\n");
    else    printf("验证失败！\n");
    return 0;
}*/

#include"headOfClient.h"

int passwdVerify(const char* usrname,const char* passwd){//密码、用户名验证,若正确，返回1，错误返回0

    struct spwd * sp;
    char ciphertext[100]={0};//密文
    ;
    puts(usrname);
    if((sp=getspnam(usrname))==NULL){
        puts("error:getspnam");
        return 0;//错误检测
    }
    strcpy(ciphertext,sp->sp_pwdp);
    char salt[100]={0};//盐值
    for(int i=0,j=0;j!=3&&ciphertext[i];i++)//获取密文中的盐值
    {
        if(ciphertext[i]=='$')
        {
            j++;
        }
        salt[i]=ciphertext[i];
      
    }
   // printf("%s\n",salt);
   if(strcmp(sp->sp_pwdp,crypt(passwd,salt))==0){//比较用户生成的密文和原密文是否相同
        puts("密码验证成功");
        return 0;
   }
   else{
       puts("密码验证失败");
       return -1;
   }
    
}