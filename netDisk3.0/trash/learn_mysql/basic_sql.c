#include <func.h>
#include "/usr/include/mysql/mysql.h"

#define BUFFER_SIZE 4096
int main(int argc,char*argv[])
{
    MYSQL *db = mysql_init(NULL);

    const char *host = "localhost";
    const char *user = "root";
    const char *password = "024680";
    const char *database = "TRASH";

    MYSQL *mysqlRet = mysql_real_connect(db, host, user, password, database, 0, NULL, 0);

    if (mysqlRet != NULL)
    {
        printf("db = %p, mysqlRet = %p\n", db, mysqlRet);
        //打印指针用%p
        
        printf("succeed!\n");
    }
    else
    {
        printf("Error: %s\n", mysql_error(db));
        exit(EXIT_FAILURE);
    }

    char sql[BUFFER_SIZE] = "update test01 set phone = '1233211234567' where name = 'Frank'";
    int sqlRet = mysql_query(db, sql);
    if (sqlRet != 0)
    {
        printf("Error: %s\n", mysql_error(db));
        exit(EXIT_FAILURE);
    }

    mysql_close(db);

    return 0;
}

