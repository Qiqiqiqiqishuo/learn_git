// 数据库的 host，userName，passwd等使用 全局变量便于修改？？？或者使用宏也好？？？
// 还好这次只是改了个密码，不然修改一条属性需要改好多次
// 暂时能用，先不管了

//"盐"和"密文"都是定时炸弹
//先把文件相关内容做完再考虑写删除文件的功能---暂不提供文件夹删除功能，仅能删除文件
//数据库同用户同名文件(MD5可能相同也可能不同)问题待文件传输时处理
#include "headOfServer.h"
#include "manipulate_mysql.h"

//先搁置，想想在cd中完成对pwd的处理貌似更加合适
int pwd_vfs(const int nfd, const char *pwd)
{
    char check_out_path[BUFFER_SIZE] = {0};
    getcwd(check_out_path, BUFFER_SIZE);
    puts(check_out_path);

    int ret;
    char msgBuf[BUFFER_SIZE] = {0};
    strcpy(msgBuf, pwd);

    train_t t = {6, "string"};
    send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
    printf("t.trainLength = %d---\n", t.trainLength);
    bzero(&t, sizeof(t));
    t.trainLength = strlen(msgBuf);
    printf("t.trainLength = %d---\n", t.trainLength);
    puts(msgBuf);
    strcpy(t.trainBody, msgBuf);
    send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

    return 0;
}

//通过用户名，pwd_id，specific（要切换到的目录）修改pwd_id
int cd_vfs(const char *usrname, int *pwd_id, char *pwd, const char *specific)
{
    const int pwd_id_static = *pwd_id;
    int upward = 0;

    if (strcmp(specific, "/") == 0)
    {
        *pwd_id = 4; //回根目录
        strcpy(pwd, "/");
        printf("pwd : ---%s---\n", pwd);
        return 0;
    }
    if (strcmp(specific, "~") == 0)
    {
        *pwd_id = 4; //回家（根目录）
        strcpy(pwd, "/");
        printf("pwd : ---%s---\n", pwd);
        return 0;
    }
    if (strcmp(specific, ".") == 0)
    {
        printf("pwd : ---%s---\n", pwd);
        return 0; //什么也不做
    }
    if (strcmp(specific, "..") == 0)
    {
        if (pwd_id_static == 4)
        {
            //在根目录了，什么也不做
            printf("已经到家了\n");
            printf("pwd : ---%s---\n", pwd);
            return -1;
        }
        upward = 1;
    }

    int ret;
    char sql[BUFFER_SIZE] = {0};
    char pwd_id_buf[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    if (upward)
    {
        sprintf(sql, "select pre_id from vft where id = %d", pwd_id_static);
    }
    else //上面的“向上”条件满足后对pwd字符串处理寄到后面了，省的以后太难看懂---这里并不知道cd是否合法，不能向上面的“向上”（已判断过合法）一样现在就处理pwd字符串了
    {
        //数据库同用户同名文件(MD5可能相同也可能不同)问题待文件传输时处理
        sprintf(sql, "select id from vft where filename = '%s' and user = '%s' and pre_id = %d", specific, usrname, pwd_id_static);
        //数据库同用户同名文件(MD5可能相同也可能不同)问题待文件传输时处理
    }

    ret = execute_sql(db, sql);
    int exist_dir = get_result_to_string(db, pwd_id_buf);
    if (exist_dir)
    { //成功切换目录
        printf("pwd_id_buf string = %s---\n", pwd_id_buf);
        printf("pwd_id_buf atoi = %d---\n", atoi(pwd_id_buf));
        *pwd_id = atoi(pwd_id_buf);

        mysql_close(db);

        //太杂乱了，以后再重构吧
        if (upward) //满足向上条件，pwd至少存在两个'/'
        {
            printf("original pwd : ---%s---\n", pwd);
            int ptr = strlen(pwd) - 2;
            while (pwd[ptr] != '/')
            {
                --ptr;
            }
            pwd[++ptr] = 0;
            printf("modified pwd : ---%s---\n", pwd);
        }
        else
        {
            printf("original pwd : ---%s---\n", pwd);
            strcat(pwd, specific);
            strcat(pwd, "/");
            printf("modified pwd : ---%s---\n", pwd);
        }

        return 0;
    }
    else
    {
        mysql_close(db);
        printf("指定目录不存在(不合法)\n");
        printf("pwd : ---%s---\n", pwd);
        return -1;
    }
}

int mkdir_vfs(const char *usrname, const int pwd_id, const char *specific)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    sprintf(sql, "insert into vft values(default, '%s', %d, 'd', '%s', default, default, default)", specific, pwd_id, usrname);
    ret = execute_sql(db, sql);
    if (ret == 0)
    {
        printf("succeed mkdir %s\n", specific);
        mysql_close(db);
        return 0;
    }
    else
    {
        printf("failed mkdir %s\n", specific);
        mysql_close(db);
        return -1;
    }
}

int ls_vfs(const char *usrname, const int pwd_id, const int nfd)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};
    char msgBuf[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0); //内已含报错，暂不处理错误

    sprintf(sql, "select filename from vft where pre_id = %d and user = '%s'", pwd_id, usrname);
    ret = execute_sql(db, sql);
    ret = get_all_result_to_string_separate_by_two_space(db, msgBuf);
    if (ret != 0)
    {
        //寄了
        mysql_close(db);
        return -1;
    }
    else
    {
        train_t t = {6, "string"};
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
        printf("t.trainLength = %d\n", t.trainLength);

        bzero(&t, sizeof(t));
        t.trainLength = strlen(msgBuf);
        printf("t.trainLength = %d\n", t.trainLength);
        puts(msgBuf);
        strcpy(t.trainBody, msgBuf);
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
    }
    mysql_close(db);
    return 0;
}

//发现bug，重写一遍应该比改这堆屎快很多，必须重写了---------重构的事情以后再说吧------弃用之，用上面这个，见头文件说明
int ls_vfs_by_usrname_pwd_nfd(const char *usrname, const char *pwd, int nfd)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};
    char msgBuf[BUFFER_SIZE] = {0};
    char curAbsPath[BUFFER_SIZE] = {0};
    char pwd_id[BUFFER_SIZE] = {0};

    getcwd(curAbsPath, 4096);
    strcat(curAbsPath, pwd);
    puts(curAbsPath); //仅用于服务端提示

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    sprintf(sql, "select id from vft where filename = '%s' and (user = '%s' or user = 'root')", pwd, usrname);
    ret = execute_sql(db, sql);
    get_result_to_string(db, pwd_id); //返回1则sql结果非NULL，返回0为NULL，这里不做检查了

    bzero(sql, BUFFER_SIZE);
    sprintf(sql, "select filename from vft where pre_id = '%d'", atoi(pwd_id));
    ret = execute_sql(db, sql);

    bzero(msgBuf, BUFFER_SIZE);
    ret = get_all_result_to_string_separate_by_two_space(db, msgBuf);
    if (ret != 0)
    {
        //寄
        mysql_close(db);
        return -1;
    }
    else
    {
        train_t t = {6, "string"};
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
        printf("t.trainLength = %d\n", t.trainLength);

        bzero(&t, sizeof(t));
        t.trainLength = strlen(msgBuf);
        printf("%d\n", t.trainLength);
        puts(msgBuf);
        strcpy(t.trainBody, msgBuf);
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
    }
    mysql_close(db);
    return 0;
}

int retrieve_check_ciphertext_by_name(const char *usrname, const char *ciphertext, int nfd)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};
    // char resBuf[BUFFER_SIZE] = {0};
    char stored_ciphertext[BUFFER_SIZE] = {0};
    char msgBuf[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    sprintf(sql, "select ciphertext from user where name = '%s'", usrname);
    ret = execute_sql(db, sql);
    get_result_to_string(db, stored_ciphertext); //返回值无意义了，不会为空，为NULL时resBuf为(NULL)

    puts("开始通过数据库中密文是否为null判断是登录还是注册");
    if (strncmp(stored_ciphertext, "(null)", 6) != 0) //对数据库的NULL不理解，这里直接cmp不成功，只cmp前六个字节可以成功，经测试cmp "(null) "也不行，好像只是打印出来的是空格，具体是什么暂时不看了
    {                                                 //对比密文，向客户端返回成功或失败
        puts("已有密文，比对密文尝试登录");
        printf("ciphertext:%s---\n", ciphertext);
        printf("store_text:%s---\n", stored_ciphertext);
        // if (strcmp(ciphertext, stored_ciphertext) == 0)
        if (strncmp(ciphertext, stored_ciphertext, 13) == 0) // 9位salt默认crypt貌似只会生成13位，出问题了再完善{;w
        {                                                    //成功
            train_t t = {6, "string"};
            send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

            printf("trainLength of type = %d\n", t.trainLength);

            bzero(&t, sizeof(t));

            strcpy(msgBuf, "登录成功");
            t.trainLength = strlen(msgBuf);
            printf("trainLength of msgBuf = %d\n", t.trainLength);
            printf("msgBuf = %s\n", msgBuf);
            strcpy(t.trainBody, msgBuf);
            send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
        }
        else
        { //密码错误，失败
            train_t t = {6, "string"};
            send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

            printf("trainLength of type = %d\n", t.trainLength);

            bzero(&t, sizeof(t));

            strcpy(msgBuf, "登录失败，请自行退出或3秒后自动退出"); //这个3s后退出是在客户端做的
            t.trainLength = strlen(msgBuf);
            printf("trainLength of msgBuf = %d\n", t.trainLength);
            printf("msgBuf = %s\n", msgBuf);
            strcpy(t.trainBody, msgBuf);
            send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

            //发送退出指令
            train_t send_exit = {4, "exit"};
            send(nfd, &send_exit, sizeof(send_exit.trainLength) + send_exit.trainLength, MSG_NOSIGNAL);

            mysql_close(db);
            return -1;
        }
    }
    else
    { //存储密文，向客户端返回成功
        puts("暂无密文，开始注册");
        sprintf(sql, "update user set ciphertext = '%s' where name = '%s'", ciphertext, usrname);
        ret = execute_sql(db, sql);

        train_t t = {6, "string"};
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

        printf("trainLength of type = %d\n", t.trainLength);

        bzero(&t, sizeof(t));

        strcpy(msgBuf, "注册并登录成功");
        t.trainLength = strlen(msgBuf);
        printf("trainLength of msgBuf = %d\n", t.trainLength);
        printf("msgBuf = %s\n", msgBuf);
        strcpy(t.trainBody, msgBuf);
        send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);
    }

    mysql_close(db);

    return 0;
}

int retrieve_send_salt_by_name(const char *usrname, int nfd)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};
    char resBuf[BUFFER_SIZE] = {0};
    char salt[123] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    sprintf(sql, "select name from user where name = '%s'", usrname);
    ret = execute_sql(db, sql);
    int registered = get_result_to_string(db, resBuf);

    if (registered)
    { //取盐
        sprintf(sql, "select salt from user where name = '%s'", usrname);
        ret = execute_sql(db, sql);
        int suc_get_salt = get_result_to_string(db, salt); // 1成功，0失败
        printf("salt = %s\n", salt);
    }
    else
    { //生盐并创建用户存盐
        int salt_gen_ret = salt_generator(salt);

        sprintf(sql, "insert into user (name, salt) values('%s', '%s')", usrname, salt);
        ret = execute_sql(db, sql);
        printf("salt = %s\n", salt);
    }

    //发盐
    train_t t = {4, "salt"};
    send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

    printf("trainLength of type = %d\n", t.trainLength);

    bzero(&t, sizeof(t));
    t.trainLength = strlen(salt);
    printf("trainLength of salt = %d\n", t.trainLength);
    printf("salt = %s\n", salt);
    strcpy(t.trainBody, salt);
    send(nfd, &t, sizeof(t.trainLength) + t.trainLength, MSG_NOSIGNAL);

    mysql_close(db);

    return 0;
}

int salt_generator(char *salt)
{
    int flag, i;
    srand((unsigned)time(NULL));
    for (i = 0; i < 9; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
        case 0:
            salt[i] = 'A' + rand() % 26;
            break;
        case 1:
            salt[i] = 'a' + rand() % 26;
            break;
        case 2:
            salt[i] = '0' + rand() % 10;
            break;
        default:
            salt[i] = 'x';
            break;
        }
    }
    salt[i] = 0;
    return 0;
}

int retrieve_user_by_name(const char *usrname)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};
    char resBuf[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "84620", "nddb", 0, NULL, 0);

    sprintf(sql, "select name from user where name = '%s'", usrname);
    ret = execute_sql(db, sql);
    int registered = get_result_to_string(db, resBuf);

    mysql_close(db);

    if (registered)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int execute_sql(MYSQL *mysql, const char *sql)
{
    int sqlRet;
    sqlRet = mysql_query(mysql, sql);
    if (sqlRet != 0)
    {
        printf("Error: %s\n", mysql_error(mysql));
        return -1;
    }
    printf("suc exec \"%s\"\n", sql);

    return 0;
}

int get_all_result_to_string_separate_by_two_space(MYSQL *mysql, char *resBuf)
{
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == NULL)
    {
        printf("Error: %s\n", mysql_error(mysql));
        return -1;
    }
    printf("the row number = %lld\n", mysql_num_rows(result));

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 0; (unsigned int)i < mysql_num_fields(result); i++)
        {
            printf("%s\t", row[i]);
            strcat(resBuf, row[i]);
            strcat(resBuf, "  ");
        }
        printf("\n");
    }
    mysql_free_result(result);
    int len = strlen(resBuf);
    if (len == 0)
    {
        printf("empty dir, operation error\n");
        return -1;
    }
    while (resBuf[len - 1] == ' ')
    {
        resBuf[len - 1] = 0;
        len--;
    }

    return 0;
}

int get_result_to_string(MYSQL *mysql, char *resBuf)
{
    MYSQL_RES *result = mysql_store_result(mysql);

    unsigned long long int rows = mysql_num_rows(result);
    printf("the row number = %llu\n", rows); //其实只取一个字段，这里方便检错

    if (rows != 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        sprintf(resBuf, "%s\t", row[0]);
        printf("resBuf = %s\n", resBuf);
        printf("suc fetch result to buffer\n");
        mysql_free_result(result);

        return 1; //结果存在，存储在resBuf中
    }
    resBuf = NULL;
    printf("resBuf = void string\n");
    printf("result is empty, buffer was set NULL\n");

    return 0; //结果不存在，resBuf被置为NULL
}

// char *get_result_to_string(MYSQL *mysql, char *resBuf)
// {
//     MYSQL_RES *result = mysql_store_result(mysql);

//     // if (result == NULL)
//     //{
//     //     resBuf = NULL;
//     //     return NULL;
//     // }
//     //不好使，An empty result set is returned if there are no rows returned. (An empty result set differs from a null
//     // pointer as a return value.)
//     //寄
//     //改用unsigned long long int rows = mysql_num_rows(MYSQL_RES *result);

//     unsigned long long int rows = mysql_num_rows(result);
//     printf("the row number = %llu\n", rows); //其实只取一个字段，这里方便检错

//     if (rows != 0)
//     {
//         MYSQL_ROW row = mysql_fetch_row(result);
//         sprintf(resBuf, "%s\t", row[0]);
//         printf("resBuf = %s\n", resBuf);
//     }
//     else
//     {
//         resBuf = NULL;
//         printf("resBuf = void string\n");
//     }

//     printf("suc fetch result to buffer\n");
//     mysql_free_result(result);

//     return resBuf;
// }

int execute_sql_output(MYSQL *mysql, const char *sql)
{
    int sqlRet;
    sqlRet = mysql_query(mysql, sql);
    if (sqlRet != 0)
    {
        printf("Error: %s\n", mysql_error(mysql));
        return -1;
    }
    printf("suc exec \"%s\"\n", sql);

    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == NULL)
    {
        printf("Error: %s\n", mysql_error(mysql));
        return -1;
    }
    printf("the row number = %lld\n", mysql_num_rows(result));

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 0; (unsigned int)i < mysql_num_fields(result); i++)
        {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }
    mysql_free_result(result);
    printf("suc fetch all result\n");

    return 0;
}

int connect_db(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
{
    // MYSQL *mysqlRet = mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
    // 这里 把 参数写死了，便于修改，
    // 如果需要修改，直接改下面这行的参数，如果各个操作需要连接不同的数据库，重新启用上面那行，调用时传入具体参数
    MYSQL *mysqlRet = mysql_real_connect(mysql, "localhost", "root", "84620", "nddb", 0, NULL, 0);
    if (mysqlRet != NULL)
    {
        printf("mysql = %p, mysqlRet = %p\n", mysql, mysqlRet);
        //打印指针用%p

        printf("connect to DB \"%s\" succeed!\n", db);
        return 0;
    }
    else
    {
        printf("Error: %s\n", mysql_error(mysql));

        return -1;
    }
}
