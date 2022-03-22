#include "headOfServer.h"
#include "manipulate_mysql.h"

int retrieve_salt_by_name(const char *usrname)
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
