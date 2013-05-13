#ifndef __A_NODE_H__
#define __A_NODE_H__

#include "hashmap.h"
#define AUTH_NODE_LEN 6000

//void setPid(unsigned long pid);
//unsigned long getPid();

typedef struct author_node
{
	unsigned long len;
	unsigned long aid[AUTH_NODE_LEN];
	struct author_node *next;
}aNode;

typedef aNode * pNode;

typedef struct alist
{
	unsigned long aid;
	struct alist *next;
	struct alist *last;
}alist;

typedef alist * pAlist;

pNode pNodeSearch(hashmap *phmap,unsigned long aid);

int pNodeAddInfo(pNode *pp,unsigned long aid);

void pNodeFree(pNode *phead);


pAlist getAuthorList(void);
void freeAuthorList(void);

#endif // __A_NODE_H__
