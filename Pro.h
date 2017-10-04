/*
 * Pro.h
 *
 *  Created on: 2017年4月2日
 *      Author: xieliming
 */

#ifndef PRO_H_
#define PRO_H_
#include <stdio.h>
#include <stdlib.h>
#include "DataS.h"
#include "SigleList.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)
#define PORT 9999
void ServerModel();
void ClientMode();
#endif /* PRO_H_ */
