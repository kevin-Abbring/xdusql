#ifndef _SQL_H
#define _SQL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


struct Createfieldsdef{
    char *field;
    char *type;
    int length;
    struct Createfieldsdef *next_fdef;
};
struct Createstruct{
    char *table;
    struct Createfieldsdef *fdef;
};

struct insertValue {
    char *value;
    struct insertValue *nextValue;
};

struct Conditions{/*条件*/
    struct  Conditions *left; //左部条件
    struct  Conditions *right; //右部条件
    char *comp_op; /* 'a'是and, 'o'是or, '<' , '>' , '=', ‘!='  */
    int type; /* 0是字段，1是字符串，2是整数 */
    char *value;/* 根据type存放字段名、字符串或整数 */
    char *table;/* NULL或表名 */
};
struct Selectedfields{/*select语句中选中的字段*/
    char *table; //字段所属表
    char *field; //字段名称
    struct Selectedfields *next_sf;//下一个字段
};
struct Selectedtables{ /*select语句中选中的表*/
    char *table; //基本表名称
    struct  Selectedtables  *next_st; //下一个表
};
struct Selectstruct{ /*select语法树的根节点*/
    struct Selectedfields *sf; //所选字段
    struct Selectedtables *st; //所选基本表
    struct Conditions *cons; //条件
};

struct Setstruct
{
    struct Setstruct *next_s;
    char *field;
    char *value;
};

void createDB();//创建数据库文件夹
void getDB();//输出.database文件
void useDB();//将目录转换为指定database

void createTable(struct Createstruct *cs_root);//将链表中的值读出并写入文件。
void getTable();//输出.tables文件
void insertInOrder(char * tableName, struct insertValue* values);//按顺序插入表

int  whereSearch(struct Conditions *conditionRoot, int totField, char allField[][64], char value[][64]);//返回wherecondition结果

void selectWhere(struct Selectedfields *fieldRoot, struct Selectedtables *tableRoot, struct Conditions *conditionRoot);
void selectNoWhere(struct Selectedfields *fieldRoot, struct Selectedtables *tableRoot);

void updateWhere(char *tableName, struct Setstruct *setRoot, struct Conditions *conditionRoot);

void deleteAll(char * tableName);//读出表头然后删除表中所有元组->空表，回写表头
void deleteWhere(char *tableName, struct Conditions *conditionRoot);//条件删除

void dropTable(char * tableName);//删表改管理文件
void dropDB();//用system函数调用rm -rf删除文件，然后复制一份.databases.tmp回写.databases时不写删除的表
void freeWhere(struct Conditions *conditionRoot);//回收condition树内存

#endif