#include <stdio.h>
#include <sqlite3.h>
int main()
{
	sqlite3 *db;
	int ret = sqlite3_open("123.db",&db);
	if(ret < 0)
	{
		fprintf(stderr,"sqlite3_open fail:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return ret;
	}
	char sql_cmd[1024]="insert into info values('1','2','3','4','5', 6);";
	char *errmsg;
	ret = sqlite3_exec(db,sql_cmd,NULL,NULL,&errmsg);
	if(ret < 0)
	{
		fprintf(stderr,"sqlite3_open fail:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		sqlite3_free(errmsg);
		return ret;
	}
	sqlite3_close(db);

	printf("Hello World!\n");
	return 0;
} 
