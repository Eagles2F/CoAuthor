#ifndef __DAO_H__
#define __DAO_H__

int dbcreatetable();
int dbconnect();

int dbaddinfo(int aid,int coaid);
int getcoau(int aid,int pool[],int len);


void dbfree();

#endif //  __DAO_H__
