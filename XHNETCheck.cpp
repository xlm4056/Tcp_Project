//============================================================================
// Name        : XHNETCheck.cpp
// Author      : xieliming
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include "Pro.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)
char ServerIp[32];
int main(int argc,char ** argv) {
	if(argc!=3)
	{
		fprintf(stderr,"the argument is less example:./XHNETCheck s 222.222.221.201\n");
		exit(1);
	}
	memset(ServerIp,0,sizeof(ServerIp));
	sprintf(ServerIp,"%s",(char*)argv[2]);
	printf("ServerIp=:%s\n",ServerIp);

	if(strcmp(argv[1],"s")==0)
	{
		ServerModel();
	}
	else if(strcmp(argv[1],"c")==0)
	{
		ClientMode();
	}
	return 0;
}
