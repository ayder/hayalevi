#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
int init_db();
int do_query();

MYSQL mysql;
MYSQL_RES *res;
MYSQL_ROW row;


int init_db()
{

    mysql_init(&mysql);
    if (!mysql_real_connect
	(&mysql, "localhost", "bbs", "bbs", "bbs", 0, NULL, 0)) {
	printf("Unable to connect to database\n");
	return -1;
    }
}

int do_query() {
    char query[80];
    sprintf(query, "SELECT id,name FROM forum");
    mysql_real_query(&mysql, query, (unsigned int) strlen(query));
    res = mysql_use_result(&mysql);
    if (res == NULL) {
	printf("query yok\n");
	return -1;
    }
    while (row = mysql_fetch_row(res))
	printf("%s %sn", row[0], row[1]);
    mysql_free_result(res);
    return 0;
}
