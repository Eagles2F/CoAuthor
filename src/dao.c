#include "dao.h"
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

const char *dbfile = "db/CoAuthor.db";
typedef sqlite3 * dbsrc;
dbsrc msrc;
int constat = 0;

typedef struct coaulimit
{
	int max;
	int now;
	int *pool;
}coaulmt;

int dbcreatetable()
{
	const char *createTableQuery ="create table CoAuthor(\
			aid integer ,\
			coaid integer ,\
			constraint pk_t1 primary key(aid,coaid))";

	sqlite3_exec(msrc,createTableQuery,0,0,0);
	return 0;
}

int dbconnect()
{
	char *err;
	const char *tquery ="select * from CoAuthor";
	// open db
	if(sqlite3_open(dbfile,&msrc) != 0) 
		fprintf(stderr,"[db][ERROR](%d):%s\n",sqlite3_errcode(msrc),err);	
	// create table if not exists
	if(sqlite3_exec(msrc,tquery,0,0,&err) == 1) dbcreatetable();
	constat = 1;
	return 1;
}


inline int dbaddinfo(int aid,int coaid)
{
	char query[1024];
	sprintf(query,"insert into CoAuthor(aid,coaid) values(%d,%d)",aid,coaid);	
	return sqlite3_exec(msrc,query,0,0,0);
}

int dbgetcoau(void *vca, int argc, char **argv, char **column)
{
	//printf("++\n");
	coaulmt *ca = (coaulmt *)vca;
	if(ca->now >= ca->max){
		fprintf(stderr,"ERROR:CAPACITY OF POOL IS NOT ENOUGH !\n"); 
		return 1;
	}
	
	ca->pool[ca->now]  = atoi(argv[0]);
	ca->now ++;
	return 0;
}




int getcoau(int aid,int pool[],int len)
{
	char query[1024];
	coaulmt ca;
	char *errmsg;
	//int status;
	sprintf(query,"select coaid from CoAuthor where aid = %d \
				order by coaid asc\
				limit %d ",aid,len);
	// limit 4 offset 2 -- from 3,4,5,6
	// order by xxx asc|desc
	
	ca.max = len;
	ca.now = 0;
	ca.pool = pool;
	
	
	sqlite3_exec(msrc,query,dbgetcoau,&ca,&errmsg);


	if(errmsg != NULL)
	{
		fprintf(stderr,"ERROR:QUERY ERROR %s::%d!\n",__FILE__,__LINE__); 
		return 0;
	}
	
	
	return ca.now;
}

void dbfree()
{
	sqlite3_close(msrc);
}


