#include <func.h>

//任务信息结构体
typedef struct task_s
{
    int netFd;
    struct task_s *next;
} task_t;
typedef struct taskQueue_s
{
    task_t *front;
    task_t *rear;
    int queueSize;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} taskQueue_t;

//线程信息结构体
typedef struct threadPool_s
{
    int workerNum;
    pthread_t *thread;     //用于保存子线程pid
    taskQueue_t taskQueue; //保存任务队列
    int isPoolRunning;
} threadPool_t;

int threadPoolInit(threadPool_t *threadPool, int workerNum);
int makeWorker(threadPool_t *pThreadPool);
void *threadFunc();
int handleEvent(int netFd);

//连接初始化
int tcpInit(char *ip, char *port, int *sockFd); //-----

// epoll操作
int epollAdd(int netFd, int epfd);
int epollDel(int netFd, int epfd);

//任务队列的进出
int taskEnqueue(taskQueue_t *pTaskQueue, int netFd);
int taskDequeue(taskQueue_t *pTaskQueue);

//火车结构体
#include <func.h>
enum
{
    FREE,
    BUSY
};
typedef struct train_s
{
    int trainLength;
    char trainBody[1024];
} train_t;

//将用户输入指令拆分成command部分和specific部分
//卫某
int commandAnalysis(const char *command, char *newCommand, char *specific);

//接收客户端cd命令进入目录，支持如下功能：1.cd .. 2. cd / 3. cd 目录名
//娄某
int dirCd(char *specific);

//接受客户端ls命令列出目录文件 文件名加空格
//高哥
int dirLs(int nfd);

//接受客户端创建目录的命令
//朱某
int dirMake(char *specific);

//接收客户端列出文件路径的命令，起始路径应为server根目录
//高哥
int dirPwd(int nfd);

//接受客户端上传的文件
//郭某
int fileUpLoad(int netFd);

//传输客户端需要下载的文件
//卫某
int fileDownLoad(int netFd, char *specific);

//删除客户端指定的文件
//朱佬
int fileRemove(char *specific);

//判断传递的是字符串还是文件，返回值0是文件，1是字符串
int isFileOrString(int netFd);