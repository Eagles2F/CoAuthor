#include "anode.h"
#include "hashmap.h"
#include <stdlib.h>

#include <stdio.h>

pAlist myalist = NULL;
/*
unsigned long mypid;

void setPid(unsigned long pid)
{
	mypid = pid;
}
unsigned long getPid()
{
	return mypid;
}
*/

int pAlistAdd(unsigned long aid)
{
	pAlist ap;
	if(myalist == NULL)
	{
		myalist = (pAlist)malloc(sizeof(alist));
		if(myalist == NULL) return 0;
		myalist->aid = aid;
		myalist->next = NULL;
		myalist->last = myalist;
	}else
	{
		ap = myalist->last;
		ap->next = (pAlist)malloc(sizeof(alist));
		if(ap->next == NULL) return 0;
		ap = ap->next;
		ap->aid = aid;
		ap->next = NULL;
		myalist->last = ap;
	}
	return 1;
}

pAlist getAuthorList()
{
	return myalist;
}

void freeAuthorList()
{
	pAlist p;
	p = myalist;
	while(myalist != NULL)
	{
		p = myalist;
		myalist = myalist -> next;
		free(p);
	}
	myalist = NULL;
}


pNode pNodeSearch(hashmap *phmap,unsigned long aid)
{
	pNode p;
	p = (pNode)hashmapGet(phmap,aid);
	if(p == NULL)
	{
		//printf("#3");fflush(NULL);
		pAlistAdd(aid);
		//printf("#4");fflush(NULL);
		p = (pNode)malloc(sizeof(aNode));
		//printf("#5");fflush(NULL);
		p->len = 1;
		//printf("#6");fflush(NULL);
		p->aid[0] = aid;
		//printf("#7");fflush(NULL);
		hashmapInsert(phmap,p,aid);
		//printf("#8");fflush(NULL);
		
	}
	return p;
}

int pNodeAddInfo(pNode *pp,unsigned long aid)
{
	pNode p = *pp;
	int i;
	if(p->len >= AUTH_NODE_LEN) return 0;
	for(i=0;i<p->len;i++) if(p->aid[i] == aid) return 1;
	p->aid[p->len] = aid;
	p->len ++;
	return 1;
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

