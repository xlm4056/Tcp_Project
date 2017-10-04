/*
 * SigleList.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: xieliming
 */
#include "SigleList.h"

siglelist::siglelist()
{
	this->first=new node;
	this->first->next=NULL;
	this->tail=this->first;
	this->length=0;
}
siglelist::~siglelist()
{
	node* p=this->first;
	for(int i=0;i<=length;++i)
	{
		node* q=p->next;
		delete p;
		p=q;
	}
	length=0;
}
bool siglelist::addsiglenode(void *val,int size)
{
	node * p=new node;
	node* q=this->tail;
	p->cds=(struct clientinfo *)val;
	p->next=NULL;

	q->next=p;
	this->tail=p;
	++length;
	printf("add siglelist successful length=:%d\n",length);
	return true;
}
bool siglelist::insertsiglelist(void * val,int size,int insertindex)
{
	node * tp=new node;
	memset(tp,0,sizeof(node));
	tp->cds=(struct clientinfo *)val;
	if(insertindex>length)
	{
		return false;
	}
	node * p=this->first;
	for(int i=0;i<insertindex;++i)                                 //previous pos
	{
		p=p->next;
	}
	node * nq=p->next;
	p->next=tp;
	tp->next=nq;
	++this->length;
	return true;
}
bool siglelist::altersiglelist(void *val,int size,int alterindex)
{
	if(alterindex>length)
	{
		return false;
	}
	node * p=this->first;
	for(int i=0;i<=alterindex;++i)
	{
		p=p->next;
	}
	p->cds=(struct clientinfo *)val;
	return true;
}

void * siglelist::getsiglelistval(int getindex)
{
	if(getindex>length)
	{
		return (void*)NULL;
	}
	node * p=this->first;
	for(int i=0;i<=getindex;++i)                                     //current pos
	{
		p=p->next;
	}
	return p->cds;
}

bool siglelist::deletesiglelist(int getindex)
{
	if(getindex>length)
	{
		return false;
	}
	node * p=this->first;
	for(int i=0;i<getindex;++i)
	{
		p=p->next;
	}
	node * tp=p->next;
	if(tp!=NULL)
	{
		node *q=tp->next;
		delete tp;
		p->next=q;
		--length;
	}
	if(length==1)
	{
		this->tail=this->first;                                               //this is very important  is to add again
	}
	return true;
}
int siglelist::getsiglelength()const
{
	return this->length;
}
void siglelist::showsiglelist()
{
	node* p=this->first->next;
	printf("length=:%d\n",this->length);
	int n=length;
	for(int i=0;i<n;++i)
	{
		if(i%9==0)
		{
			printf("\n");
		}
//		printf("i=:%d val:%s ",i,p->buf);
		p=p->next;
	}
	printf("\n");
}
node * siglelist::getfirstnode()const
{
	return this->first;
}
