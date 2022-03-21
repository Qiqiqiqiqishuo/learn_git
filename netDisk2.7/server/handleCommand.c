#include "headOfServer.h"
int dirCd(char *specific,char *pwd,char *usrname){
    if(strcmp(specific,"..")==0){//若cd ..之后出了用户目录则回溯
        char pwd_copy[100]={0};//用来回溯
        int idx=0;
        for(int i=0;i<strlen(pwd);++i){
            if(pwd[i]=='/')
                idx=i;
        }
        if(idx==0){
            puts("没有权限，切换目录失败");
            return -1;
        }else{
            strncpy(pwd_copy,pwd,idx);
            strcpy(pwd,pwd_copy);
            puts("切换目录成功");
            return 0;
        }
    }else{
        char temp[100];
        //strcpy(temp,pwd);
        puts("here");
        puts(pwd);
        sprintf(temp,"%s/%s",pwd,specific);//拼接当前工作目录和要切换的路径
        char pwd_copy[100]={0};
        strcpy(pwd_copy,pwd);
        strcpy(pwd,temp);
        puts(temp);


        char p[100] = {0};
        char p_copy[100]={0};//存储原来的工作路径，方便回退
        strcpy(p_copy,p);

        getcwd(p,sizeof(p));
        strcpy(p_copy,p);
        int length=strlen(p)+1;//用来定位名称是否正确
        strcat(p,pwd);
        printf("destination:%s\n",p);
        //puts(p);

        printf("before chdir,cwd = %s\n", pwd);
        int ret = chdir(p);
        if(ret==-1) {
            puts("切换失败");
            puts(pwd_copy);
            strcpy(pwd,pwd_copy);
            return -1;
        }
        ERROR_CHECK(ret, -1, "chdir");
        
        chdir(p_copy);//切换成功，主线程返回原来的工作路径
    }
}

int dirLs(int nfd,char *pwd){
char buf[4096] = {0};
    char curPath[1234] = {0};
    getcwd(curPath, 1234);
    strcat(curPath,pwd);
    puts(curPath);
    DIR *dirp = opendir(curPath);
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
    int len = strlen(buf);
    if(len == 0){
        printf("empty dir, operation error\n");
        return -1;
    }
    while (buf[len - 1] == ' ')
    {
        buf[len - 1] = 0;
        len--;
    }
    // buf末尾没有换行，如有需要可添加

    train_t t={6,"string"};
        send(nfd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文发送数据类型
        //train_t send_string = {6, "string"};
        //int ret = send(nfd, &send_string, sizeof(send_string.trainLength) + send_string.trainLength, MSG_NOSIGNAL);
        //错误处理待讨论
        printf("%d\n",t.trainLength);

        memset(&t,0,sizeof(t));
        t.trainLength=strlen(buf);
        printf("%d\n",t.trainLength);
        puts(buf);
        strcpy(t.trainBody,buf);
        send(nfd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
    //错误处理待讨论
    return 0;
}


int dirPwd(int nfd,char *pwd)
{
    char temp[100]={0};
    getcwd(temp,sizeof(temp));//获取当前工作路径
    puts(temp);

    char buf[4096] = {0};
    //strcpy(buf,pwd);
    strcpy(buf,pwd);

        train_t t={6,"string"};
        send(nfd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文发送数据类型

        printf("%d\n",t.trainLength);

        memset(&t,0,sizeof(t));
        t.trainLength=strlen(buf);
        printf("%d\n",t.trainLength);
        puts(buf);
        strcpy(t.trainBody,buf);
        send(nfd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);

        return 0;
}

/*int dirPwd(int netFd){
    char buf[1024]={0};
    char *ret=getcwd(buf,sizeof(buf));
    //ERROR_CHECK(ret,NULL,"getcwd");
    puts(buf);
   
    train_t t={6,"string"};
    send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);//发送文发送数据类型

    memset(&t,0,sizeof(t));
    t.trainLength=strlen(buf);
    printf("%d\n",t.trainLength);
    puts(buf);
    strcpy(t.trainBody,buf);
    send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
    return 0;
}*/

int dirMake(const char* specific)
/*
	本函数用于创建目录文件，传入文件名
	成功返回0，失败返回-1
*/
{
	//ret参数用于检错
	int ret;
	//文件权限设置为仅有拥有者可读可写
    char temp[100]={0};
    getcwd(temp,sizeof(temp));
    puts(temp);
    strcat(temp,specific);
	ret = mkdir(temp, 0700);
	//ERROR_CHECK具体实现等待小组商议决定
	if(ret==-1)
	{
        puts("mkdir wrong");
		return -1;
	}
	else
	{
		printf("dirMake error:%s",strerror(ret));
		return 0;
	}
}

int fileRemove(const char* specific)
/*
	本函数用于删除文件，传入文件名
	成功返回0，失败打印错误信息并返回-1
*/
{
	//ret参数用于检验错误
	int ret;
	
	//使得specifi文件的连接数-1，直到进程退出后删除
	ret = unlink(specific);
	
	//函数的返回
	if(ret==0)
	{
		return 0;
	}
	else
	{
		printf("fileRemove error:%s",strerror(ret));
		return -1;
	}
}

void login(int netFd,char *pwd,char *specific,int *loginState){//specific是用户名
    printf("用户名：%s\n",specific);
    struct spwd * sp;
    bzero(&sp,sizeof(sp));
    char ciphertext[100]={0};//密文
    if((sp=getspnam(specific))==NULL){
        puts("用户未注册");
        //发送用户未注册
        char buf[100];
        strcpy(buf,"用户未注册");
        train_t t;
        memset(&t,0,sizeof(t));
        t.trainLength=strlen(buf);
        printf("火车长度：%d\n",t.trainLength);
        puts(buf);
        strcpy(t.trainBody,buf);
        send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
        //错误检测
    }else{//用户存在
        strcpy(ciphertext,sp->sp_pwdp);
        char salt[100]={0};//盐值
        for(int i=0,j=0;j!=3&&ciphertext[i];i++){//获取密文中的盐值
            if(ciphertext[i]=='$')
            {
                j++;
            }
            salt[i]=ciphertext[i];
        
        }
        //发送盐
        char buf[100];
        strcpy(buf,salt);
        train_t t;

        memset(&t,0,sizeof(t));
        t.trainLength=strlen(buf);
        printf("火车长度：%d\n",t.trainLength);
        puts(buf);
        strcpy(t.trainBody,buf);
        send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
        //接收加盐后的密码
        int dataLength;
        char secret[1024] = {0};
        int ret =recvn(netFd,&dataLength,sizeof(int));
        //ERROR_CHECK(ret,-1,"recv");
        ret = recvn(netFd,secret, dataLength);
        printf("数据长度%d\n",dataLength);
        puts(secret);
        //判断加盐后是否正确
        if(strcmp(sp->sp_pwdp,secret)==0){
            memset(&t,0,sizeof(t));
            memset(buf,0,sizeof(buf));
            strcpy(buf,"密码正确,登录成功");
            t.trainLength=strlen(buf);
            printf("火车长度：%d\n",t.trainLength);
            puts(buf);
            strcpy(t.trainBody,buf);
            send(netFd,&t,sizeof(t.trainLength)+ t.trainLength,MSG_NOSIGNAL);
            //密码正确则进入登录状态
            char temp[100]={0};
            
            strcpy(temp,pwd);
            sprintf(pwd,"/%s",specific);
            mkdir(specific,0777);
            *loginState=1;
            return ;
        }
        return ;
    }
}