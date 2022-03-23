#include <func.h>



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

int mkdir_vfs(const char *usrname, const int pwd_id, const char *specific)
{
    int ret;
    char sql[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db, "localhost", "root", "024680", "nddb", 0, NULL, 0);

    sprintf(sql, "insert into vft values(default, '%s', %d, 'd', '%s', default, default, default)", specific, pwd_id, usrname);
    ret = execute_sql(db, sql);
    if (ret == 0)
    {
        printf("succeed mkdir %s\n", specific);
        return 0;
    }
    else
    {
        printf("failed mkdir %s\n", specific);
        return -1;
    }
}

int main(int argc,char*argv[])
{
    int ret = mkdir_vfs("lionel", 4, "testDir0_2");
    return 0;
}
