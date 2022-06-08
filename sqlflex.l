%option caseless
%{
#include <stdio.h>
#include "sqlyacc.tab.h"
//gcc dbfunction.c sqlyacc.tab.c  lex.yy.c -O0 -o minisql -std=c99
//strdup()主要是拷贝字符串s的⼀个副本，由函数返回值返回，这个副本有⾃⼰的内存空间，和s不相⼲。
%}

%%

CREATE         return CREATE;
SHOW           return SHOW;
DATABASES      return DATABASES;
DATABASE       return DATABASE;
TABLE          return TABLE;
TABLES         return TABLES;
INSERT         return INSERT;
SELECT         return SELECT;
UPDATE         return UPDATE;
DELETE         return DELETE;
DROP           return DROP;
EXIT           return EXIT;
USE            return USE;

CHAR           return CHAR;
INT            return INT;

FROM           return FROM;
SET            return SET;
WHERE          return WHERE;
AND            return AND;
OR             return OR;
VALUES         return VALUES;
INTO           return INTO;

[0-9]+                 {yylval.yych=strdup(yytext);return NUMBER;}
[A-Za-z][A-Za-z0-9_]*  {yylval.yych=strdup(yytext);return ID;}

"*"     |
"."     |
"="     |
";"     |
"("     |
")"     |
"<"     |
">"     |
"!"     |
","     {return yytext[0]; }


\n
[ \t]+
%%
