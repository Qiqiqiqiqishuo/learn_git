// salt_generator 也放在这里了
#include <func.h>

//  ----------初始化MYSQL对象，连接数据库----------
//  begin:  MYSQL *mysql = mysql_init(NULL);
//          ret = connect_db(xxx);
//  ---------------------------------------------
//
//  ----------因查询并打印结果可以用暂时保留-------
//  ----------开始具体操作并返回结果-------------
//  ----------可能存在设计失误需要把 "操作并返回结果" 拆分成 "操作" 与 "各类型的返回结果"
//  1. strcpy(sql, 命令字符串，注意加单引号);
//  2. ret = execute_sql_output(db, sql);
//  ----------1. 具体操作结束-----------------------
//
//
//  ----------关闭数据库-------------------------
//  end:    mysql_close(db);
//  ------------------------------------------------

int salt_generator(char *salt);                 //为0成功，没想到怎么失败，salt分配了123字节的栈空间在retrieve_salt_by_name中
int retrieve_salt_by_name(const char *usrname); // 1.1 注册过获取盐 1.2 没注册过生成盐并存数据库 2. 向客户端返回盐

//这个函数设计短了，应该连上 查盐或生盐存储 向客户端返回盐的操作 不然要多次连接断连数据库 不太合适的样子
int retrieve_user_by_name(const char *usrname); //存在名为usrname的用户返回1，否则返回0

int get_result_to_string(MYSQL *mysql, char *resBuf); //返回1结果存在并存在resBuf中，返回0结果不存在，resBuf被置位空指针
// char *get_result_to_string(MYSQL *mysql, char *result);                                                                                                                      //设计为只读取只有一个字段的结果，将结果保存到result字符串，返回指向result第一个字节的char*指针
int execute_sql(MYSQL *mysql, const char *sql);                                                                                                                              //执行sql语句
int execute_sql_output(MYSQL *mysql, const char *sql);                                                                                                                       //暂时搁置，耦合且可能不通用，见上大块注释
int connect_db(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag); //连接数据库（此前要先初始化一个MYSQL对象）