#include <func.h>

int retrieve_user_by_name(const char *usrname);
int execute_sql(MYSQL *mysql, const char *sql);
int connect_db(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
int get_result_to_string(MYSQL *mysql, char *resBuf);

int main(int argc,char*argv[])
{   // $ ./retrieve_user_by_name name
    ARGS_CHECK(argc, 2);
    if (retrieve_user_by_name(argv[1]))
    {
        printf("user %s exist\n", argv[1]);
    }
    else
    {
        printf("user %s don't exist\n", argv[1]);
    }
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
int get_result_to_string(MYSQL *mysql, char *resBuf)
{
    MYSQL_RES *result = mysql_store_result(mysql);

    //if (result == NULL)
    //{
    //    resBuf = NULL;
    //    return NULL;
    //}
    //不好使，An empty result set is returned if there are no rows returned. (An empty result set differs from a null
    //pointer as a return value.)
    //寄
    //改用unsigned long long int rows = mysql_num_rows(MYSQL_RES *result);

    unsigned long long int rows = mysql_num_rows(result);
    printf("the row number = %llu\n", rows); //其实只取一个字段，这里方便检错

    if (rows != 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        sprintf(resBuf, "%s\t", row[0]);
        printf("resBuf = %s\n", resBuf);
        printf("suc fetch result to buffer\n");
        mysql_free_result(result);

        return 1;//结果存在
    }
    resBuf = NULL;
    printf("resBuf = void string\n");
    printf("result is empty, buffer was set NULL\n");

    return 0;//结果不存在
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

int retrieve_user_by_name(const char *usrname)
{    
    int ret;
    char sql[BUFFER_SIZE] = {0};
    char resBuf[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db,"localhost", "root", "024680", "nddb", 0, NULL, 0);

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
