#include <func.h>

//  ----------初始化MYSQL对象，连接数据库----------
//  1. MYSQL *mysql = mysql_init(NULL);
//  2. ret = connect_db(xxx);
//
//  ----------因查询并打印结果可以用暂时保留-------
//  ----------开始具体操作并返回结果-------------
//  ----------可能存在设计失误需要把 "操作并返回结果" 拆分成 "操作" 与 "各类型的返回结果"
//  3. strcpy(sql, 命令字符串，注意加单引号);
//  4. ret = execute_sql_output(db, sql);
//  ----------具体操作结束-----------------------
//
//  ----------关闭数据库-------------------------
//  5. mysql_close(db);

int execute_sql_output(MYSQL *mysql, const char *sql);
int connect_db(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);