#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libcsv.h"
#include "filter.h"

#define AUTH_NODE_LEN 1024

typedef struct author_node
{
	int len;
	int aid[AUTH_NODE_LEN];
	struct author_node *next;
}aNode;

typedef aNode * pNode;

typedef struct single_paper
{
	int pid;
	int len;
	int aid[AUTH_NODE_LEN];
}sPaper;

typedef struct pa_info
{
	int pid;
	sPaper pinfo;
	int aid;
	int line;
	int col;//column
	pNode head;
}paInfo;

pNode pNodeSearch(pNode *phead,int aid)
{
	pNode p;
	if(*phead == NULL)
	{
		*phead = (pNode)malloc(sizeof(aNode));
		p = *phead;
		p->len = 1;
		p->aid[0] = aid;
		p->next = NULL; 
	}else
	{
		p = *phead;
		while(p->next != NULL)
		{
			if(p->aid[0] == aid)
			{
				return p;
			}
			p = p->next;
		}
		if(p->aid[0] == aid) return p;
		p->next = (pNode)malloc(sizeof(aNode));
		p = p->next;
		p->len = 1;
		p->aid[0] = aid;
		p->next = NULL;
	}
	return p;
}

void pNodeAddInfo(pNode *pp,int aid)
{
	pNode p = *pp;
	int i;
	for(i=0;i<p->len;i++) if(p->aid[i] == aid) return;
	p->aid[p->len] = aid;
	p->len ++;
}

void pNodeFree(pNode *phead)
{
	pNode p1,p2;
	p1 = *phead;
	while(p1 != NULL)
	{
		p2 = p1;
		p1 = p1->next;
		free(p2);
	}
	*phead = NULL;
}


// pid , aid , name , aff 
void hlog (void *data, size_t len, void *t)
{
	paInfo* pi = (paInfo *)t;
	char str[len];
	int ival;
	
	if(!pi->line)
	{
		if(!pi->col)
		{
			pi->line++;
		}
		pi->col++;
	}else
	{
		memcpy(str,data,len);
		ival = atoi(str);
		switch(pi->col)
		{
			case 0:
				pi->pid = ival;
				//printf("pid:%d\n",pi->pid);
				break;
			case 1:
				pi->aid = ival;
				break;
		}
		pi->col++;
	}	
}

void nlog (int c, void *t)
{
	paInfo* pi = (paInfo *)t;
	pNode p1,p2;
	int i;
	//printf("id:%d id:%d\n",pi->pid,pi->pinfo.pid);
	if(!pi->line || !isNecessary(pi->aid))
	{
		pi->line ++;
		pi->col = 0;
		return;
	}
	if(pi->pid == pi->pinfo.pid) // in one paper
	{
		p1 =  pNodeSearch(&(pi->head),pi->aid);
		for(i=0;i<pi->pinfo.len;i++)
		{
			p2 =  pNodeSearch(&(pi->head),pi->pinfo.aid[i]);
			if((p1->len)>=AUTH_NODE_LEN||(p2->len)>=AUTH_NODE_LEN) 
			{
				fprintf(stderr,"error! %d and %d  -- %d",p1->len,p2->len,__LINE__);
				getchar();
			}
			//p1->aid[p1->len] = pi->pinfo.aid[0];
			//p1->len ++;
			pNodeAddInfo(&p1,pi->pinfo.aid[i]);
			pNodeAddInfo(&p2,pi->aid);
		}
		pi->pinfo.aid[pi->pinfo.len]=pi->aid;
		pi->pinfo.len ++;
	}else // not in one paper
	{
		pi->pinfo.pid = pi->pid;
		pi->pinfo.aid[0]=pi->aid;
		pi->pinfo.len = 1;
	}

	pi->line++;
	pi->col = 0;
	printf("L:%d\n",pi->line);
}

void writedata(pNode phead,FILE *fp)
{
	pNode p = phead->next;
	int i;
	while(p!=NULL)
	{
		fprintf(fp,"%d",p->aid[0]);
		for(i=1;i<p->len;i++)
		{
			fprintf(fp,",%d",p->aid[i]);
		}
		fprintf(fp,"\n");
		p = p->next;
	}
}




int main (int argc,char *argv[]) {
	FILE *fp;
	struct csv_parser *p;
	char buf[1024];
	size_t bytes_read;
	FILE *fo;
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
	printf("init \n");
	t.pid = 0;
	t.pinfo.pid = 0;
	t.aid = 0;
	t.line = 0;
	t.col = 0;
	t.head = NULL;
	
	printf("reading csv file \n");
	while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
		if (csv_parse(p, buf, bytes_read, hlog, nlog,&t) != bytes_read) {
			fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(p)));
		}
	}

	fo = fopen("CoAuthor.csv","w");

	if(fo == NULL) fprintf(stderr, "Failed to open CoAuthor.csv\n");
	printf("writing ... ");	
	writedata(t.head,fo);

	
	csv_free(p);
	pNodeFree(&(t.head));
	fclose(fp);	
		
	return 0;
}


