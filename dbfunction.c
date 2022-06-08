#include "dbfunction.h"
/*y文件里*/
extern char database[64];
extern char rootDir[128];
//输出.database文件
void getDB()
{
    FILE* fstream;
    char name[40];

    chdir(rootDir);
    fstream = fopen(".databases", "r");
    if(fstream == NULL)
    {
        printf("\nError!\n");
        return;
    }
    while(fscanf(fstream, "%s", name) != EOF)
    {
        printf("%s\n", name);
    }
    fclose(fstream);
    chdir(rootDir);
    printf("XDUSQL->");
}
//将目录转换为指定database
void useDB()
{
    char dir[128]={0};
    strcpy(dir, rootDir);
    strcat(dir, "/");
    strcat(dir, database);
    if(chdir(dir) == -1)
        printf("\nError!\n");
    else
    {
        printf("Current Database: \n%s\n", database);
        chdir("rootDir");
    }
    printf("XDUSQL->");
}

void createDB()
{   
    /* Change the process's working directory to PATH.  */
    chdir(rootDir);
    printf("%s", database);//不加这句会gdb会报错
    //read write execute
    if(mkdir(database, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
        printf("\n enter 2 !\n");   
    else
    {
        FILE* fstream;
        fstream = fopen(".databases", "a+");
        if(fstream == NULL)
        {
            printf("\n enter 3 !\n");
            return;
        }
        else
        {
            fprintf(fstream, "%s\n", database);
            printf("\nCreate database %s succeed!\n", database);
            fflush(fstream);
            fclose(fstream);
        }
    }
    strcpy(database, "\0");
    chdir(rootDir);
    printf("XDUSQL->");
}
//用system函数调用rm -rf删除文件，然后复制一份.databases.tmp回写.databases时不写删除的表
void dropDB()
{
    chdir(rootDir);
    if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        char cmd[128]="rm -rf ";
        FILE* filein;
        FILE* fileout;
        char dbname[64] = {0};

        chdir("..");
        strcat(cmd, database);//删除数据库文件
        system(cmd);

        system("mv .databases .databases.tmp");
        filein = fopen(".databases.tmp", "r");
        fileout = fopen(".databases", "w");

        while(fscanf(filein, "%s", dbname) != EOF)
        {
            if(strcmp(dbname, database) != 0)
            {
                fprintf(fileout, "%s\n", dbname);
            }
        }
        fclose(filein);
        fclose(fileout);
        system("rm .databases.tmp");
    }
    chdir(rootDir);
    printf("Drop database %s succeed.\n", database);
    printf("XDUSQL->");
}

void createTable(struct Createstruct *cs_root)
{
    
    int tot = 0, i = 0;
    struct Createfieldsdef * fieldPointer = NULL;
    char rows[64][64]={0};

    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\n enter 2 !\n");
    else if(chdir(database) == -1)
        printf("\n enter 3 !\n");
    else
    {
        //检查表是否存在
        FILE* ftables;
        ftables = fopen(".tables", "a+");
        if(ftables == NULL)
        {
            printf("\n enter 4 !\n");
        }
        else
        {
            if(access(cs_root->table, F_OK) != -1)
            {
                printf("Table already exist!\n");
                printf("\n enter 5 !\n");
            }
            else {
                fprintf(ftables, "%s\n", cs_root->table);
                fclose(ftables);

                fieldPointer = cs_root->fdef;
                FILE* ftable;
                ftable = fopen(cs_root->table, "a+");
                if(ftable == NULL)
                {
                    printf("\nError!\n");
                    printf("\n enter 6 !\n");
                }
                else {
                    //列读入rows，链表是逆序的（左递归导致先插入的在右边）
                    while(fieldPointer != NULL)
                    {
                        strcpy(rows[tot], fieldPointer->field);
                        tot ++;
                        fieldPointer = fieldPointer->next_fdef;
                    }
                    //再把rows->ftable
                    fprintf(ftable, "%d\n", tot);//列数
                    //列名（倒序才能恢复链表逆序）
                    for(i = tot - 1; i >= 0; i--)
                        fprintf(ftable, "%s\n", rows[i]);
                    printf("\nCreate table %s succeed, %d row(s) created.\n", cs_root->table, tot);
                    fclose(ftable);
                }
            }
            chdir(rootDir);
        }
    }

    fieldPointer = cs_root->fdef;
    while(fieldPointer != NULL)
    {
        struct Createfieldsdef * fieldPointertmp = fieldPointer;
        fieldPointer = fieldPointer->next_fdef;
        free(fieldPointertmp);
    }
    free(cs_root);
    chdir(rootDir);
    printf("XDUSQL->");
}

void getTable()
{
    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        FILE* fstream;
        char name[40];

        fstream = fopen(".tables", "a+");
        if(fstream == NULL)
        {
            printf("\nError!\n");
            return;
        }
        while(fscanf(fstream, "%s", name) != EOF)
        {
            printf("%s\n", name);
        }
        fclose(fstream);
    }
    chdir(rootDir);
    printf("XDUSQL->");
}

void dropTable(char * tableName)
{
    //切换路径: rootDir//database
    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        if(access(tableName, F_OK) != -1)
        {
            //删表
            char cmd[128]="rm -rf ";
            char tbname[64] = {0};
            FILE* filein;
            FILE* fileout;

            strcat(cmd, tableName);
            system(cmd);
            //改管理
            system("mv .tables .tables.tmp");
            filein = fopen(".tables.tmp", "r");
            fileout = fopen(".tables", "w");

            while(fscanf(filein, "%s", tbname) != EOF)
            {
                if(strcmp(tbname, tableName) != 0)
                {
                    fprintf(fileout, "%s\n", tbname);
                }
            }
            fclose(filein);
            fclose(fileout);
            system("rm .tables.tmp");
        }
        else
            printf("Table %s doesn't exist!\n", tableName);
    }
    chdir(rootDir);
    printf("Drop table %s succeed.\n", tableName);
    printf("XDUSQL->");
}

void insertInOrder(char * tableName, struct insertValue* values)
{
    //切换到当前database
    chdir(rootDir);
    char valueChar[64][64] = {0};
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {   
        if(access(tableName, F_OK) != -1)//检查表存在性，F_OK 只判断是否存在
        {
            //打开表文件
            FILE* fileTable;
            int tot = 0, i = 0;
            fileTable = fopen(tableName, "a+");
            struct insertValue* valuesTmp = values;
            //读出要插入的value
            while(valuesTmp != NULL)
            {
                strcpy(valueChar[tot], valuesTmp->value);
                tot ++;
                valuesTmp = valuesTmp->nextValue;
            }
            //写表
            for (i = tot-1; i >= 0; i--)
            {
                fprintf(fileTable, "%s\n", valueChar[i]);
            }
            fclose(fileTable);
            printf("Insert succeed.\n");
        }
        else
            printf("Table %s doesn't exist!\n", tableName);
    }
    //回收空间
    while(values != NULL)
    {
        struct insertValue* valuesTmp = values;
        values = values->nextValue;
        free(valuesTmp);
    }
    chdir(rootDir);
    printf("XDUSQL->");
}

void deleteAll(char * tableName)
{
    chdir(rootDir);
    char rows[64][64] = {0};
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        if(access(tableName, F_OK) != -1)
        {
            char cmd[128]="rm -rf ";
            FILE* filein;
            FILE* fileout;
            int tot = 0, i = 0;

            filein = fopen(tableName, "r");
            //读出表头
            fscanf(filein, "%d", &tot);
            for (i = 0; i < tot; ++i)
            {
                fscanf(filein, "%s", rows[i]);
                //printf("%s\n", rows[i]);
            }

            fclose(filein);
            strcat(cmd, tableName);
            system(cmd);

            fileout = fopen(tableName, "a+");
            //回写表头
            fprintf(fileout, "%d\n", tot);
            for (i = 0; i < tot; ++i)
            {
                fprintf(fileout, "%s\n", rows[i]);
            }
            fclose(fileout);
            printf("Delete succeed.\n");
        }
        else
            printf("Table %s doesn't exist!\n", tableName);
    }
    printf("XDUSQL->");
    chdir(rootDir);
}

//生成一个.tmp文件进行拼接打印，之后删除
void selectNoWhere(struct Selectedfields *fieldRoot, struct Selectedtables *tableRoot)
{
    int totTable = 0, totField = 0, i = 0, j = 0, k = 0;
    char tableName[64][64] = {0}, fieldName[64][64] = {0};
    struct Selectedfields *fieldTmp = fieldRoot;
    struct Selectedtables *tableTmp = tableRoot;

    //判断表存在性
    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {   //读选择的表，统计数量，名字放入tableName
        while(tableTmp != NULL)
        {
            strcpy(tableName[totTable], tableTmp->table);
            tableTmp = tableTmp->next_st;
            totTable ++;
        }
        if (fieldRoot == NULL)//没选择列，全部打印
        {
            int flag = 1;
            //检测表存在性
            for (i = totTable-1; i >= 0; --i)
            {
                if(access(tableName[i], F_OK) == -1)
                {
                    printf("Table %s doesn't exist!\n", tableName[i]);
                    flag = 0;
                    break;
                }
            }
            if (flag && totTable == 1)//表存在且数量等于1
            {
                FILE* filein;
                int tot = 0, i = 0;
                char value[64];

                filein = fopen(tableName[0], "r");
                fscanf(filein, "%d", &tot);
                i = tot;
                while(fscanf(filein, "%s", value) != EOF)
                {
                    printf("%*s", 16, value);
                    i--;
                    if (i == 0)
                    {
                        i = tot;
                        printf("\n");
                    }
                }
                fclose(filein);
                printf("Select succeed.\n");
            }
            else if (flag && totTable == 2)//表的数量大于1则需要进行迪卡尔积拼接
            {
                FILE * fileTable[64];//打开所有表
                FILE * fileTmp;
                int fieldCount[2] = {0}, valueCount[2] = {0};
                char values1[64][64] = {0}, values2[64][64] = {0};
                char tmp[64];
                strcpy(tmp, tableName[0]);
                strcpy(tableName[0], tableName[1]);
                strcpy(tableName[1], tmp);
                fileTmp = fopen(".tmp", "w");
                //打开所有表
                for (i = 0; i < totTable; ++i)
                {
                    fileTable[i] = fopen(tableName[i], "r");
                }
                //统计总列数
                totField = 0;
                for (i = 0; i < totTable; ++i)
                {
                    fscanf(fileTable[i], "%d", &fieldCount[i]);
                    totField += fieldCount[i];
                }
//总表fileTmp
                //总表的列数就是统计的总列数
                fprintf(fileTmp, "%d\n", totField);
                //读第一个表的列放入总表
                for (i = 0; i < fieldCount[0]; ++i)
                {
                    fscanf(fileTable[0], "%s", tmp);
                    fprintf(fileTmp, "%s\n", tmp);
                }
                //读第二个表的列放入总表
                for (i = 0; i < fieldCount[1]; ++i)
                {
                    fscanf(fileTable[1], "%s", tmp);
                    fprintf(fileTmp, "%s\n", tmp);
                }
//复制值 valueCount[]放复制的行数，将所有行读入values[]
                while(fscanf(fileTable[0], "%s", tmp) != EOF)
                {
                    strcpy(values1[valueCount[0]], tmp);
                    valueCount[0]++;
                }
                while(fscanf(fileTable[1], "%s", tmp) != EOF)
                {
                    strcpy(values2[valueCount[1]], tmp);
                    valueCount[1]++;
                }
//进行笛卡尔积 valueCount[]/fieldCount[]为表的元组数量，将values[0]和values[1]拼接为fileTmp行
                for (i = 0; i < valueCount[0]/fieldCount[0]; ++i)
                {
                    for (k = 0; k < valueCount[1]/fieldCount[1]; ++k)
                    {
                        for (j = 0; j < fieldCount[0]; ++j)
                        {
                            fprintf(fileTmp, "%s\n", values1[i*fieldCount[0]+j]);
                        }
                        for (j = 0; j < fieldCount[1]; ++j)
                        {
                            fprintf(fileTmp, "%s\n", values2[k*fieldCount[1]+j]);
                        }
                    }
                }
                fclose(fileTmp);
                fclose(fileTable[0]);
                fclose(fileTable[1]);
                fileTmp = fopen(".tmp", "r");
                i = totField;
                fscanf(fileTmp, "%d", &totField);
                while(fscanf(fileTmp, "%s", tmp) != EOF)
                {
                    printf("%*s", 16, tmp);
                    i--;
                    if (i == 0)
                    {
                        i = totField;
                        printf("\n");
                    }
                }
                fclose(fileTmp);
                system("rm -rf .tmp");
                printf("Select succeed.\n");
            }
        }
        else//选择列了，打印的时候注意点
        {
            int flag = 1;
            char allField[64][64] = {0};
            for (i = totTable-1; i >= 0; --i)
            {
                if(access(tableName[i], F_OK) == -1)
                {
                    printf("Table %s doesn't exist!\n", tableName[i]);
                    flag = 0;
                    break;
                }
            }
            if (flag && totTable == 1)
            {
                FILE* filein;
                int tot = 0, i = 0, j = 0;
                char value[64];
                int isField[64] = {0};

                while(fieldTmp != NULL)
                {
                    strcpy(fieldName[totField], fieldTmp->field);
                    fieldTmp = fieldTmp->next_sf;
                    totField ++;
                }

                filein = fopen(tableName[0], "r");
                fscanf(filein, "%d", &tot);
                for (i = 0; i < tot; ++i)
                {
                    fscanf(filein, "%s", allField[i]);
                    for (j = 0; j < totField; ++j)
                    {
                        if (strcmp(allField[i], fieldName[j]) == 0)
                        {
                            isField[i] = 1;
                            break;
                        }
                    }
                }
                for (i = 0; i < tot; ++i)
                {
                    if (isField[i])
                        printf("%*s", 16, allField[i]);
                }
                printf("\n");

                i = tot;
                while(fscanf(filein, "%s", value) != EOF)
                {
                    if (isField[tot - i])
                    {
                        printf("%*s", 16, value);
                    }
                    i--;
                    if (i == 0)
                    {
                        i = tot;
                        printf("\n");
                    }
                }
                fclose(filein);
                printf("Select succeed.\n");
            }
            else if (flag && totTable == 2)
            {
                FILE * fileTable[64];
                FILE * fileTmp;
                int fieldCount[2] = {0}, valueCount[2] = {0}, isField[64] = {0};
                char values1[64][64] = {0}, values2[64][64] = {0};
                char tmp[64];
                int tot = 0;
                strcpy(tmp, tableName[0]);
                strcpy(tableName[0], tableName[1]);
                strcpy(tableName[1], tmp);
                fileTmp = fopen(".tmp", "w");
                for (i = 0; i < totTable; ++i)
                {
                    fileTable[i] = fopen(tableName[i], "r");
                }
                totField = 0;
                for (i = 0; i < totTable; ++i)
                {
                    fscanf(fileTable[i], "%d", &fieldCount[i]);
                    totField += fieldCount[i];
                }

                fprintf(fileTmp, "%d\n", totField);
                for (i = 0; i < fieldCount[0]; ++i)
                {
                    fscanf(fileTable[0], "%s", tmp);
                    fprintf(fileTmp, "%s\n", tmp);
                }
                for (i = 0; i < fieldCount[1]; ++i)
                {
                    fscanf(fileTable[1], "%s", tmp);
                    fprintf(fileTmp, "%s\n", tmp);
                }

                while(fscanf(fileTable[0], "%s", tmp) != EOF)
                {
                    strcpy(values1[valueCount[0]], tmp);
                    valueCount[0]++;
                }
                while(fscanf(fileTable[1], "%s", tmp) != EOF)
                {
                    strcpy(values2[valueCount[1]], tmp);
                    valueCount[1]++;
                }

                for (i = 0; i < valueCount[0]/fieldCount[0]; ++i)
                {
                    for (k = 0; k < valueCount[1]/fieldCount[1]; ++k)
                    {
                        for (j = 0; j < fieldCount[0]; ++j)
                        {
                            fprintf(fileTmp, "%s\n", values1[i*fieldCount[0]+j]);
                        }
                        for (j = 0; j < fieldCount[1]; ++j)
                        {
                            fprintf(fileTmp, "%s\n", values2[k*fieldCount[1]+j]);
                        }
                    }
                }
                fclose(fileTmp);
                fclose(fileTable[0]);
                fclose(fileTable[1]);
                fileTmp = fopen(".tmp", "r");

                while(fieldTmp != NULL)
                {
                    strcpy(fieldName[totField], fieldTmp->field);
                    fieldTmp = fieldTmp->next_sf;
                    totField ++;
                }

                fscanf(fileTmp, "%d", &tot);
                for (i = 0; i < tot; ++i)
                {
                    fscanf(fileTmp, "%s", allField[i]);
                    for (j = 0; j < totField; ++j)
                    {
                        if (strcmp(allField[i], fieldName[j]) == 0)
                        {
                            isField[i] = 1;
                            break;
                        }
                    }
                }
                for (i = 0; i < tot; ++i)
                {
                    if (isField[i])
                        printf("%*s", 16, allField[i]);
                }
                printf("\n");

                i = tot;
                while(fscanf(fileTmp, "%s", tmp) != EOF)
                {
                    if (isField[tot - i])
                    {
                        printf("%*s", 16, tmp);
                    }
                    i--;
                    if (i == 0)
                    {
                        i = tot;
                        printf("\n");
                    }
                }
                fclose(fileTmp);
                system("rm -rf .tmp");
                printf("Select succeed.\n");
            }
        }
    }

    fieldTmp = fieldRoot;
    tableTmp = tableRoot;
    while(fieldRoot != NULL)
    {
        fieldTmp = fieldRoot;
        fieldRoot = fieldRoot->next_sf;
        free(fieldTmp);
    }
    while(tableRoot != NULL)
    {
        tableTmp = tableRoot;
        tableRoot = tableRoot->next_st;
        free(tableTmp);
    }
    chdir(rootDir);
    printf("XDUSQL->");
}

//回收condition内存
void freeWhere(struct Conditions *conditionRoot)
{
    if (conditionRoot->left != NULL)
        freeWhere(conditionRoot->left);
    else if (conditionRoot->right != NULL)
        freeWhere(conditionRoot->right);
    else
        free(conditionRoot);
}
//根据condition对左右子树值进行判断，返回是否满足条件
int whereSearch(struct Conditions *conditionRoot, int totField, char allField[][64], char value[][64])
{
    char comp_op = *(conditionRoot->comp_op);

    if (comp_op == 'a')
    {
        return whereSearch(conditionRoot->left, totField, allField, value) && whereSearch(conditionRoot->right, totField, allField, value);
    }
    else if (comp_op == 'o')
    {
        return whereSearch(conditionRoot->left, totField, allField, value) || whereSearch(conditionRoot->right, totField, allField, value);
    }
    else
    {
        int field = 0;
        for (int i = 0; i < totField; ++i)
        {
            if (strcmp(allField[i], conditionRoot->left->value) == 0)
            {
                field = i;
                break;
            }
        }
        if (comp_op == '=')
        {
            if (strcmp(value[field], conditionRoot->right->value) == 0)
            {
                return 1;
            }
            else
                return 0;
        }
        else if (comp_op == '>')
        {
            if (conditionRoot->right->type == 0)
            {
                return 0;
            }
            else if (atoi(value[field]) > atoi(conditionRoot->right->value))
            {
                return 1;
            }
            else
                return 0;
        }
        else if (comp_op == '<')
        {
            if (conditionRoot->right->type == 0)
            {
                return 0;
            }
            else if (atoi(value[field]) < atoi(conditionRoot->right->value))
            {
                return 1;
            }
            else
                return 0;
        }
        else if (comp_op == '!')
        {
            if (conditionRoot->right->type == 0)
            {
                return 0;
            }
            else if (atoi(value[field]) != atoi(conditionRoot->right->value))
            {
                return 1;
            }
            else
                return 0;
        }
    }
}

void selectWhere(struct Selectedfields *fieldRoot, struct Selectedtables *tableRoot, struct Conditions *conditionRoot)
{
    int totTable = 0, totField = 0, i = 0;
    int flag = 1;
    char allField[64][64] = {0};
    char tableName[64][64] = {0}, fieldName[64][64] = {0};
    struct Selectedfields *fieldTmp = fieldRoot;
    struct Selectedtables *tableTmp = tableRoot;
    struct Conditions *conditionTmp = conditionRoot;

    chdir(rootDir);

    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        while(tableTmp != NULL)
        {
            strcpy(tableName[totTable], tableTmp->table);
            tableTmp = tableTmp->next_st;
            totTable ++;
        }

        for (i = totTable-1; i >= 0; --i)
        {
            if(access(tableName[i], F_OK) == -1)
            {
                printf("Table %s doesn't exist!\n", tableName[i]);
                flag = 0;
                break;
            }
        }
        if (flag && totTable == 1)
        {
            FILE* filein;
            int tot = 0, i = 0, j = 0;
            char values[64][64] = {0};
            int isField[64] = {0};

            while(fieldTmp != NULL)
            {
                strcpy(fieldName[totField], fieldTmp->field);
                fieldTmp = fieldTmp->next_sf;
                totField ++;
            }

            filein = fopen(tableName[0], "r");
            fscanf(filein, "%d", &tot);

            if (fieldRoot != NULL)
            {
                for (i = 0; i < tot; ++i)
                {
                    fscanf(filein, "%s", allField[i]);
                    for (j = 0; j < totField; ++j)
                    {
                        if (strcmp(allField[i], fieldName[j]) == 0)
                        {
                            isField[i] = 1;
                            break;
                        }
                    }
                }
            }
            else
            {
                for (i = 0; i < tot; ++i)
                {
                    fscanf(filein, "%s", allField[i]);
                    isField[i] = 1;
                }
            }
            for (i = 0; i < tot; ++i)
            {
                if (isField[i])
                    printf("%*s", 16, allField[i]);
            }
            printf("\n");

            int end = 1;
            for (i = 0; ; ++i)
            {
                int conditionFlag = 0;
                end = 1;

                for (j = 0; j < tot; ++j)
                {
                    if(fscanf(filein, "%s", values[j]) == EOF)
                    {
                        end = 0;
                        break;
                    }
                }
                if (end == 0)
                {
                    break;
                }

                conditionFlag = whereSearch(conditionRoot, tot, allField, values);
                if (conditionFlag)
                {
                    for (j = 0; j < tot; ++j)
                    {
                        if (isField[j])
                            printf("%*s", 16, values[j]);
                    }
                    printf("\n");
                }
            }

            fclose(filein);
            printf("Select succeed.\n");
        }
        else if (flag && totTable == 2)
        {
            FILE * fileTable[64];
            FILE * fileTmp;
            int fieldCount[2] = {0}, valueCount[2] = {0};
            char values1[64][64] = {0}, values2[64][64] = {0};
            char tmp[64];
            int tot = 0, i = 0, j = 0, k = 0;
            char values[64][64] = {0};
            int isField[64] = {0};

            strcpy(tmp, tableName[0]);
            strcpy(tableName[0], tableName[1]);
            strcpy(tableName[1], tmp);
            fileTmp = fopen(".tmp", "w");
            for (i = 0; i < totTable; ++i)
                fileTable[i] = fopen(tableName[i], "r");

            totField = 0;
            for (i = 0; i < totTable; ++i)
            {
                fscanf(fileTable[i], "%d", &fieldCount[i]);
                totField += fieldCount[i];
            }

            fprintf(fileTmp, "%d\n", totField);
            for (i = 0; i < fieldCount[0]; ++i)
            {
                fscanf(fileTable[0], "%s", tmp);
                fprintf(fileTmp, "%s\n", tmp);
            }
            for (i = 0; i < fieldCount[1]; ++i)
            {
                fscanf(fileTable[1], "%s", tmp);
                fprintf(fileTmp, "%s\n", tmp);
            }

            while(fscanf(fileTable[0], "%s", tmp) != EOF)
            {
                strcpy(values1[valueCount[0]], tmp);
                valueCount[0]++;
            }
            while(fscanf(fileTable[1], "%s", tmp) != EOF)
            {
                strcpy(values2[valueCount[1]], tmp);
                valueCount[1]++;
            }

            for (i = 0; i < valueCount[0]/fieldCount[0]; ++i)
            {
                for (k = 0; k < valueCount[1]/fieldCount[1]; ++k)
                {
                    for (j = 0; j < fieldCount[0]; ++j)
                    {
                        fprintf(fileTmp, "%s\n", values1[i*fieldCount[0]+j]);
                    }
                    for (j = 0; j < fieldCount[1]; ++j)
                    {
                        fprintf(fileTmp, "%s\n", values2[k*fieldCount[1]+j]);
                    }
                }
            }
            fclose(fileTmp);
            fclose(fileTable[0]);
            fclose(fileTable[1]);
            fileTmp = fopen(".tmp", "r");

            fscanf(fileTmp, "%d", &tot);
            while(fieldTmp != NULL)
            {
                strcpy(fieldName[totField], fieldTmp->field);
                fieldTmp = fieldTmp->next_sf;
                totField ++;
            }

            if (fieldRoot != NULL)
            {
                for (i = 0; i < tot; ++i)
                {
                    fscanf(fileTmp, "%s", allField[i]);
                    for (j = 0; j < totField; ++j)
                    {
                        if (strcmp(allField[i], fieldName[j]) == 0)
                        {
                            isField[i] = 1;
                            break;
                        }
                    }
                }
            }
            else
            {
                for (i = 0; i < tot; ++i)
                {
                    fscanf(fileTmp, "%s", allField[i]);
                    isField[i] = 1;
                }
            }
            for (i = 0; i < tot; ++i)
            {
                if (isField[i])
                    printf("%*s", 16, allField[i]);
            }
            printf("\n");

            int end = 1;
            for (i = 0; ; ++i)
            {
                int conditionFlag = 0;
                end = 1;

                for (j = 0; j < tot; ++j)
                {
                    if(fscanf(fileTmp, "%s", values[j]) == EOF)
                    {
                        end = 0;
                        break;
                    }
                }
                if (end == 0)
                {
                    break;
                }

                conditionFlag = whereSearch(conditionRoot, tot, allField, values);
                if (conditionFlag)
                {
                    for (j = 0; j < tot; ++j)
                    {
                        if (isField[j])
                            printf("%*s", 16, values[j]);
                    }
                    printf("\n");
                }
            }

            fclose(fileTmp);
            system("rm -rf .tmp");
            printf("Select succeed.\n");
        }
    }

    fieldTmp = fieldRoot;
    tableTmp = tableRoot;
    conditionTmp = conditionRoot;
    while(fieldRoot != NULL)
    {
        fieldTmp = fieldRoot;
        fieldRoot = fieldRoot->next_sf;
        free(fieldTmp);
    }
    while(tableRoot != NULL)
    {
        tableTmp = tableRoot;
        tableRoot = tableRoot->next_st;
        free(tableTmp);
    }
    freeWhere(conditionRoot);
    chdir(rootDir);
    printf("XDUSQL->");
}

void deleteWhere(char *tableName, struct Conditions *conditionRoot)
{
    int i = 0, j = 0, totField = 0;//表列数
    char allField[64][64] = {0};//所有列
    char field[64][64] = {0};
    struct Conditions *conditionTmp = conditionRoot;

    //判断表存在性
    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        if(access(tableName, F_OK) == -1)
        {
            printf("Table %s doesn't exist!\n", tableName);
        }
        else
        {   //复制原表为 xxx.tmp临时文件，用条件筛选临时文件回写原表，最后删除临时文件
            FILE* filein;
            FILE* fileout;
            int end = 1;
            char cp[64] = "cp ";//cp tablename tablename.tmp
            char rm[64] = "rm -rf ";//rm -rf tablename.tmp
            char tableTmp[64] = {0};//tablename.tmp
            strcpy(tableTmp, tableName);
            strcat(tableTmp, ".tmp");
            strcat(cp, tableName);
            strcat(cp, " ");
            strcat(cp, tableTmp);
            strcat(rm, tableTmp);
            system(cp);
            //tableTmp == tableName
            filein = fopen(tableTmp, "r");
            fileout = fopen(tableName, "w");

            //从tmp回写表头
            fscanf(filein, "%d", &totField);
            fprintf(fileout, "%d\n", totField);
            for (int i = 0; i < totField; ++i)
            {
                fscanf(filein, "%s", allField[i]);
                fprintf(fileout, "%s\n", allField[i]);
            }

            //
            for (i = 0; ; ++i)
            {
                int conditionFlag = 0;
                end = 1;

                for (j = 0; j < totField; ++j)
                {
                    if(fscanf(filein, "%s", field[j]) == EOF)
                    {
                        end = 0;
                        break;
                    }
                }
                if (end == 0)
                {
                    break;
                }
                //condition全由whereSearch处理，返回是否符合条件
                conditionFlag = whereSearch(conditionRoot, totField, allField, field);
                if (!conditionFlag)//不符合删除条件的回写
                {
                    for (j = 0; j < totField; ++j)
                    {
                        fprintf(fileout, "%s\n", field[j]);
                    }
                }
            }
            fclose(fileout);
            fclose(filein);
            system(rm);
            printf("Delete succeed.\n");
        }
    }
    free(tableName);
    freeWhere(conditionRoot);
    printf("XDUSQL->");
    chdir(rootDir);
}

void updateWhere(char *tableName, struct Setstruct *setRoot, struct Conditions *conditionRoot)
{
    int i = 0, j = 0, totField = 0, totSet = 0, changeFlag = 0;
    char allField[64][64] = {0};
    char field[64][64] = {0};
    char allSet[64][64] = {0};
    char setValue[64][64] = {0};
    struct Setstruct * setTmp = setRoot;
    struct Conditions *conditionTmp = conditionRoot;

    chdir(rootDir);
    if(strlen(database) == 0)
        printf("\nNo database, error!\n");
    else if(chdir(database) == -1)
        printf("\nError!\n");
    else
    {
        if(access(tableName, F_OK) == -1)
        {
            printf("Table %s doesn't exist!\n", tableName);
        }
        else
        {
            //创建临时文件，根据条件回写源文件
            FILE* filein;
            FILE* fileout;
            int end = 1;
            char cp[64] = "cp ";
            char rm[64] = "rm -rf ";
            char tableTmp[64] = {0};
            strcpy(tableTmp, tableName);
            strcat(tableTmp, ".tmp");
            strcat(cp, tableName);
            strcat(cp, " ");
            strcat(cp, tableTmp);
            strcat(rm, tableTmp);
            
            //把set结构体中的列名和要更改的值读出来
            totSet = 0;
            while(setTmp != NULL)
            {
                strcpy(allSet[totSet], setTmp->field);
                strcpy(setValue[totSet], setTmp->value);
                totSet++;
                setTmp = setTmp->next_s;
            }

            system(cp);

            filein = fopen(tableTmp, "r");
            fileout = fopen(tableName, "w");

            fscanf(filein, "%d", &totField);
            fprintf(fileout, "%d\n", totField);
            for (int i = 0; i < totField; ++i)
            {
                fscanf(filein, "%s", allField[i]);
                fprintf(fileout, "%s\n", allField[i]);
            }

            for (i = 0; ; ++i)
            {
                int conditionFlag = 0;
                end = 1;

                for (j = 0; j < totField; ++j)
                {
                    if(fscanf(filein, "%s", field[j]) == EOF)
                    {
                        end = 0;
                        break;
                    }
                }
                if (end == 0)
                {
                    break;
                }

                conditionFlag = whereSearch(conditionRoot, totField, allField, field);
                if (!conditionFlag)
                {
                    for (j = 0; j < totField; ++j)
                    {
                        fprintf(fileout, "%s\n", field[j]);
                    }
                }
                else
                {//符合条件的用改写值回写
                    for (j = 0; j < totField; ++j)
                    {
                        changeFlag = 0;
                        for (int k = 0; k < totSet; ++k)
                        {
                            if (strcmp(allSet[k], allField[j]) == 0)
                            {
                                fprintf(fileout, "%s\n", setValue[k]);
                                changeFlag = 1;
                                break;
                            }
                        }
                        if (!changeFlag)
                        {
                            fprintf(fileout, "%s\n", field[j]);
                        }
                    }
                }
            }
            fclose(fileout);
            fclose(filein);
            system(rm);
            printf("Update succeed.\n");
        }
    }
    free(tableName);
    freeWhere(conditionRoot);
    setTmp = setRoot;
    while(setRoot != NULL)
    {
        setTmp = setRoot;
        setRoot = setRoot->next_s;
        free(setTmp);
    }
    printf("XDUSQL->");
    chdir(rootDir);
}
