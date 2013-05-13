#include "filter.h"
#include "libcsv.h"
#include <string.h>

int aidlen;
int aids[14615];

int intcmp(const void *a,const void *b)
{
	// i > j return >0
	// get 0 , 1 ,2 ....100
	return (*(int*)a) - (*(int *)b);
}

void col(void *data, size_t len, void *t)
{
	int *id = (int *)t;
	char str[len+1];
	int ival;
	memcpy(str,data,(int)len);
	str[len]='\0';
	ival = atoi(str);
	aids[*id] = ival;
	(*id)++;
}

void row(int c, void *t){}


int loadSameNameUnoverlapped()
{
	FILE *fp;
	int len=0;
	struct csv_parser *p;
	char buf[1024];
	size_t bytes_read;
	const char *file_name="data/SameNameUnoverlapped.csv";
	if((p=(struct csv_parser *)malloc(sizeof(struct csv_parser))) == 0) return -1;
	if ((fp = fopen(file_name,"r"))== NULL)
		fprintf(stderr, "Failed to open %s\n",file_name);
	
	csv_init(p, (unsigned char)0);
	while ((bytes_read=fread(buf, 1, 1024, fp)) > 0)
		if (csv_parse(p, buf, bytes_read, col, row,&len) != bytes_read)
		{
			fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(p)));
		}
	qsort(aids,len,sizeof(int),intcmp);
	aidlen = len;
	csv_free(p);
	fclose(fp);
	
	for(int i=0;i<100;i++) printf("%d | ",aids[i]);
	printf("\n%d",aidlen);
	return 1;
}

int isNecessary(int aid)
{
	int low, high, mid;  
	low = 0;  
	high = aidlen-1;  
	while(low <= high)  
	{  
		mid = (low + high)/2;
		if(aid < aids[mid]) high = mid - 1;  
		else if(aid > aids[mid])low = mid + 1;  
		else return 1;  
	}
	return 0; 
}




