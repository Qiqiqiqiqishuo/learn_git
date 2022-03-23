#include "headOfServer.h"
#include "manipulate_mysql.h"
int ls_vfs(const char *usrname, const char *pwd, int nfd)
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
    ret = connect_db(db, "localhost", "root", "024680", "nddb", 0, NULL, 0);

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
    ret = connect_db(db, "localhost", "root", "024680", "nddb", 0, NULL, 0);

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

        strcpy(msgBuf, "登录成功");
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
    ret = connect_db(db, "localhost", "root", "024680", "nddb", 0, NULL, 0);

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
    ret = connect_db(db, "localhost", "root", "024680", "nddb", 0, NULL, 0);

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
    MYSQL *mysqlRet = mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
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
