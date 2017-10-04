/*
 * SigleList.h
 *
 *  Created on: Apr 20, 2017
 *      Author: xieliming
 */

#ifndef SIGLELIST_H_
#define SIGLELIST_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)

struct clientinfo
{
	int cfd;
	struct sockaddr_in caddr;
	bool run_flag;
	bool read_flags;
	int heartstrace;
	int oldheartstrace;
	int timeoutcnt;
};
struct node
{
	struct clientinfo *cds;
	node * next;
};

class siglelist
{
public:
	siglelist();
	~siglelist();
	bool addsiglenode(void * val,int size);
	bool insertsiglelist(void *val,int size,int insertindex);
	bool altersiglelist(void *val,int size,int getindex);
	void * getsiglelistval(int getindex);
	bool deletesiglelist(int getindex);
	void showsiglelist();
	int getsiglelength()const;
	node * getfirstnode()const;
private:
	node *first;               //head's postion
	node* tail;				   //tail's postion
	int length;
};
#endif /* SIGLELIST_H_ */
