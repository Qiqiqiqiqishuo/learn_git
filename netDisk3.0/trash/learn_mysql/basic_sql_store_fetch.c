#include <func.h>

int execute_sql_output(MYSQL *mysql, const char *sql);
int connect_db(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);

int main(int argc,char*argv[])
{
    int ret;
    char sql[BUFFER_SIZE] = {0};

    MYSQL *db = mysql_init(NULL);
    ret = connect_db(db,"localhost", "root", "024680", "TRASH", 0, NULL, 0);
    //const char *host = "localhost";
    //const char *user = "root";
    //const char *password = "024680";
    //const char *database = "TRASH";

    strcpy(sql,"select * from test01");
    ret = execute_sql_output(db, sql);

    mysql_close(db);

    return 0;
}

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
    while ( (row = mysql_fetch_row(result)) )
    {
        for (int i=0; (unsigned int)i<mysql_num_fields(result); i++)
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