#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libcsv.h"
#include "dao.h"
#include "hashmap.h"
#include "anode.h"
#include "filter.h"

//#define PARTDATA 0
//#define AUTH_NODE_LEN 1024




typedef struct single_paper
{
	unsigned long pid;
	unsigned long len;
	unsigned long aid[AUTH_NODE_LEN];
}sPaper;

typedef struct pa_info
{
	unsigned long pid;
	sPaper pinfo;
	unsigned long aid;
	unsigned long line;
	int col;//column
	hashmap *phmap;
}paInfo;


// pid , aid , name , aff 
void hlog (void *data, size_t len, void *t)
{
	paInfo* pi = (paInfo *)t;
	char str[20];
	unsigned long ival;
	
	if(pi->line == 0)
	{
		pi->col++;
	}else
	{
		switch(pi->col)
		{
			case 0:
				memcpy(str,data,len);
				str[len]='\0';
				ival = atol(str);
				pi->pid = ival;
				//printf("{pid:%ld}",pi->pid);
				break;
			case 1:
				memcpy(str,data,len);
				str[len]='\0';
				ival = atol(str);
				pi->aid = ival;
				//printf("{aid:%ld}",pi->aid);
				break;
		}
		pi->col++;
	}	
}


void printStruct(hashmap *phmap)
{
	pNode np;
	int plen;
	int i;
	int j;
	unsigned long count;
	pAlist ap = getAuthorList();
	//dbaddinfo(pi->aid,pi->pinfo.aid[i]);//
	//dbaddinfo(pi->pinfo.aid[i],pi->aid);
	
	count = hashmapCount(phmap);
	printf(" \ncount:%ld\n",count);
	for(j=0;j<count && ap != NULL;j++)
	//while(ap != NULL)
	{
		printf("%ld | ",ap->aid);
		if((np=hashmapGet(phmap,(unsigned long)(ap->aid))) != NULL)
		{
			plen = np->len;
			for(i=0;i<plen;i++)
			{
				printf("%ld ",np->aid[i]);
			}
		}
		
		printf("\n");
		ap = ap->next;
	}
	printf("==============================================\n");
	getchar();
}

void nlog (int c, void *t)
{
	paInfo* pi = (paInfo *)t;
	pNode p1,p2;
	int i;
	if(pi->line == 0 )
	{
		(pi->line) ++;
		pi->col = 0;
		printf("LA:%ld\n",pi->line);
		return;
	}
	//printf("A");fflush(NULL);
	if(pi->pid == pi->pinfo.pid) // in one paper
	{
		int f1 = isNecessary(pi->aid);
		f1 = 1;
		
		if(f1) p1 =  pNodeSearch(pi->phmap,pi->aid);		
		for(i=0;i<pi->pinfo.len;i++)
		{
			int f2 = isNecessary(pi->pinfo.aid[i]);
			f2 = 1;
			//dbaddinfo(pi->aid,pi->pinfo.aid[i]);//
			//dbaddinfo(pi->pinfo.aid[i],pi->aid);
			if(f2)  
				p2 =  pNodeSearch(pi->phmap,pi->pinfo.aid[i]);			
		
			if(f1)
				pNodeAddInfo(&p1,pi->pinfo.aid[i]);
				
			if(f2)  
				pNodeAddInfo(&p2,pi->aid);
		
		}
		
		pi->pinfo.aid[pi->pinfo.len]=pi->aid;
		(pi->pinfo.len) ++;
		//printStruct(pi->phmap);
	}else // not in one paper
	{
		pi->pinfo.pid = pi->pid;
		pi->pinfo.aid[0]=pi->aid;
		pi->pinfo.len = 1;
	}
	(pi->line)++;
	pi->col = 0;
	printf("LB:%ld\n",pi->line);
}

void storeintodb(hashmap *phmap)
{
	pNode np;
	int plen;
	int i;
	int j;
	unsigned long count;
	pAlist ap = getAuthorList();
	dbconnect();
	//dbaddinfo(pi->aid,pi->pinfo.aid[i]);//
	//dbaddinfo(pi->pinfo.aid[i],pi->aid);
	FILE *fp;
	if((fp = fopen("s.txt","w")) == NULL)
	{
		fprintf(stderr,"error opening s.txt\n");
	}
	
	count = hashmapCount(phmap);
	printf(" count:%ld\n",count);
	for(j=0;j<count && ap != NULL;j++)
	//while(ap != NULL)
	{
		printf("[%d:%ld]",j,ap->aid);fflush(NULL);
		if((np=hashmapGet(phmap,(unsigned long)(ap->aid))) != NULL)
		{
			plen = np->len;
			for(i=1;i<plen;i++)
			{
				dbaddinfo(np->aid[0],np->aid[i]);
			}
		}
		ap = ap->next;
	}
	dbfree();
	
	fclose(fp);
}




int main (int argc,char *argv[]) {
	FILE *fp;
	struct csv_parser *p;
	char buf[1024];
	size_t bytes_read;
	const char *file_name="data/PaperAuthor.csv";
	paInfo t;
	loadSameNameUnoverlapped();
	

	if((p=(struct csv_parser *)malloc(sizeof(struct csv_parser))) == 0) return -1;
	
	if ((fp = fopen(file_name,"r"))== NULL)
	{
		fprintf(stderr, "Failed to open %s\n",file_name);
	}

	csv_init(p, (unsigned char)0);
	
	//int pid;
	//sPaper pinfo;
	//int aid;
	//int line;
	//int col;//column
	//pNode head;
	t.pid = 0;
	t.pinfo.pid = 0;
	t.aid = 0;
	t.line = 0;//0;
	t.col = 0;
	
	printf("init \n");
	//t.phmap = hashmapCreate((unsigned long)1000000);
	t.phmap = hashmapCreate((unsigned long)1000);
	
	printf("reading csv file \n"); fflush(NULL);
	while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
		if (csv_parse(p, buf, bytes_read, hlog, nlog,&t) != bytes_read) {
			fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(p)));
		}
	}

	//fo = fopen("CoAuthor.csv","w");

	//if(fo == NULL) fprintf(stderr, "Failed to open CoAuthor.csv\n");
	//printf("writing ... ");	
	//writedata(t.head,fo);
	
	printf("store into db\n"); fflush(NULL);
	
	storeintodb(t.phmap);
	
	printf("ok\n");

	freeAuthorList();
	csv_free(p);
	fclose(fp);	
	
	printf("test\n");
	
	// DB test
	
	int a[300];
	int alen;
	//test 2230280
	dbconnect();
	alen = getcoau(972575,a,300);
	printf("len is:%d",alen);
	for(int i=0;i<alen;i++)
		printf("%d ",a[i]);
	dbfree();
	
	hashmapDelete(t.phmap);
		
	return 0;
}


