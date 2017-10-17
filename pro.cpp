/*
 * pro.c
 *
 *  Created on: 2017年4月2日
 *      Author: xieliming
 */
#include "Pro.h"
#define PACKSIZE 2*1024
extern char ServerIp[32];
pthread_mutex_t heart_mutex;
siglelist clientlist;
int curcnt=0;
void * heartThread(void * arg);
void * acceptThread(void * arg);
void * readwriteThread(void * arg);
void ServerModel()
{
	printf("\033[2J");                                                     //清屏
	printf("------------Servers Model-------------\n");
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr((const char*)ServerIp);
	saddr.sin_port=htons(PORT);

	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1)
	{
		fprintf(stderr,"socket was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	const int opt=1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt))==-1)
	{
		fprintf(stderr,"setsockopt SO_REUSEADDR was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	if(bind(sfd,(struct sockaddr*)&saddr,sizeof(saddr))==-1)
	{
		fprintf(stderr,"bind saddr was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	if(listen(sfd,20)==-1)
	{
		fprintf(stderr,"listen sfd was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	pthread_mutex_init(&heart_mutex,NULL);                                  //initalize heart_mutex to protect
	//create a thread to check heart
	pthread_t heart;
	if(pthread_create(&heart,NULL,heartThread,NULL)!=0)                     //新建心跳检测线程
	{
		fprintf(stderr,"pthread_create heart was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	//create a thread to check server fd
	pthread_t acpt;
	if(pthread_create(&acpt,NULL,acceptThread,&sfd)!=0)                     //新建Accept线程等待客户端连接
	{
		fprintf(stderr,"pthread_create acpt was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	if(pthread_detach(heart)!=0)
	{
		fprintf(stderr,"pthread_detach heart was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
	if(pthread_join(acpt,NULL)!=0)
	{
		fprintf(stderr,"pthread_create acpt was failed:%s\n",strerror(errno));
		close(sfd);
		exit(1);
	}
}
void * acceptThread(void * arg)
{
	int sfd=*(int *)arg;
	fd_set acpset;
	FD_ZERO(&acpset);
	struct timeval timeout;
	while(1)
	{
		FD_SET(sfd,&acpset);
		timeout.tv_sec=0;
		timeout.tv_usec=10000;
		int res=select(sfd+1,&acpset,NULL,NULL,&timeout);              //可读之后再accept
		if(res==0||res<0)
		{
			//timeout and ouccors error
			continue;
		}
		else if(FD_ISSET(sfd,&acpset))
		{
			struct sockaddr_in taddr;
			memset(&taddr,0,sizeof(taddr));
			int addrlen=sizeof(struct sockaddr_in);
			int s=accept(sfd,(struct sockaddr*)&taddr,(socklen_t *)&addrlen);
			if(s<0)
			{
				//ouccors error
				continue;
			}
			else
			{
				struct clientinfo *cinfo=new clientinfo;
				cinfo->cfd=s;
				cinfo->caddr=taddr;
				cinfo->run_flag=true;
				cinfo->heartstrace=0;
				cinfo->timeoutcnt=0;
				cinfo->oldheartstrace=0;
				cinfo->read_flags=true;
				pthread_mutex_lock(&heart_mutex);
				clientlist.addsiglenode(cinfo,sizeof(struct clientinfo));
				pthread_mutex_unlock(&heart_mutex);
				printf("%s connect successful\n",inet_ntoa(taddr.sin_addr));
				++curcnt;
				//create a thread to read and write
				pthread_t rwt;
				if(pthread_create(&rwt,NULL,readwriteThread,cinfo)!=0)          //新建读写线程处理该客户端
				{
					fprintf(stderr,"pthread_create rwt was failed:%s\n",strerror(errno));
					close(s);
				}
				if(pthread_detach(rwt)!=0)
				{
					fprintf(stderr,"pthread_detach rwt was failed:%s\n",strerror(errno));
					close(s);
				}
			}
		}
	}
	return (void *)NULL;
}
void * readwriteThread(void * arg)
{
	struct clientinfo *cinfo=(struct clientinfo*)arg;
	int cfd=cinfo->cfd;
	fd_set rwset;
	FD_ZERO(&rwset);
	struct timeval timeout;
	char buf[1024]={0};
	while(cinfo->run_flag)
	{
		FD_ZERO(&rwset);
		FD_SET(cfd,&rwset);
		timeout.tv_sec=0;
		timeout.tv_usec=100000;
		int res=select(cfd+1,&rwset,NULL,NULL,&timeout);
		if(res==0||res<0)
		{
			//timeout and ouccors error
			continue;
		}
		else if(FD_ISSET(cfd,&rwset))
		{
			memset(buf,0,sizeof(buf));
			int s=recv(cfd,buf,sizeof(buf),0);
			if(s<=0)
			{
				// recv was failed
				continue;
			}
			else
			{
				//recv successful
				pthread_mutex_lock(&heart_mutex);
				++cinfo->heartstrace;
				cinfo->oldheartstrace=cinfo->heartstrace;
				cinfo->timeoutcnt=0;
				printf("\tread buf=:%s  from:%s\n",buf,inet_ntoa(cinfo->caddr.sin_addr));
				pthread_mutex_unlock(&heart_mutex);
				printf("\033[s");
				printf("\033[80;120H\033[0m");
				printf("CurConnectCnt=:%d\n",curcnt);
				printf("\033[u");
			}
		}
	}
	printf("%s Read Thread Exit\n",inet_ntoa(cinfo->caddr.sin_addr));
	cinfo->read_flags=false;
	return (void *)NULL;
}
void * heartThread(void * arg)
{
	while(1)
	{
		pthread_mutex_lock(&heart_mutex);
		node* p=clientlist.getfirstnode();
		p=p->next;
		if(p!=NULL)
		{
			int length=clientlist.getsiglelength();
			for(int i=0;i<length;++i)
			{
				if(p!=NULL)
				{
					if(p->cds->oldheartstrace!=0&&(p->cds->oldheartstrace==p->cds->heartstrace))
					{
						if(p->cds->timeoutcnt>10)
						{
							p->cds->run_flag=false;
							while(p->cds->read_flags){}
							printf("%s close connect %d...........\n",inet_ntoa(p->cds->caddr.sin_addr),i);
							clientlist.deletesiglelist(i);
							--curcnt;
							break;
						}
						else
						{
							++p->cds->timeoutcnt;
						}
					}
				}
				else
				{
					break;
				}
//				printf("heart check %d trace=:%d cfd=:%d\n",i,p->cds->heartstrace,p->cds->cfd);
				p=p->next;
			}
		}
		pthread_mutex_unlock(&heart_mutex);
		usleep(500000);
	}
	return (void*)NULL;
}

///////////////////////////////////////////////////////////////////
//			               Client  Model                         //
///////////////////////////////////////////////////////////////////
bool sendflag=true;
void SendConfFrame(void * msg,int ctype)
{
	printf("+++++++++++SendConfFrame+++++++++++\n");
	ConfFrame* cf=(ConfFrame*)msg;
	set16(cf->sync,12);
	set16(cf->sync,13);      //4-6       0-7   8-15         121314
	cf->num_pmu=1;
	cf->framesize=sizeof(ConfFrame);
	cf->cfs1.annmr=2;
	cf->cfs1.phnmr=2;
	cf->cfs1.dgnmr=2;

	memset(cf->cfs1.chnam_ph,0,sizeof(cf->cfs1.chnam_ph));
	memset(cf->cfs1.chnam_dg,0,sizeof(cf->cfs1.chnam_dg));
	memset(cf->cfs1.chnam_an,0,sizeof(cf->cfs1.chnam_an));
	unsigned char *devn=(unsigned char *)(cf->cfs1.chnam_an)+5;
	unsigned char *devn1=(unsigned char *)(cf->cfs1.chnam_an)+21;
	unsigned char * tn=(unsigned char*)(cf->cfs1.chnam_an)+13;
	unsigned char * tn1=(unsigned char*)(cf->cfs1.chnam_an)+29;
	memcpy(devn,"1#机",sizeof("1#机"));
	memcpy(devn1,"1#机",sizeof("2#机"));
	switch(ctype)
	{
	case 0:
	{
		memcpy(tn,"OOP",sizeof("OOP"));
		memcpy(tn1,"OOQ",sizeof("OOQ"));
		break;
	}
	case 1:
	{
		memcpy(tn,"OOQ",sizeof("OOQ"));
		memcpy(tn1,"ODF",sizeof("ODF"));
		break;
	}
	case 2:
	{
		memcpy(tn,"ODF",sizeof("ODF"));
		memcpy(tn1,"OOP",sizeof("OOP"));
		break;
	}
	}
	unsigned char * tp=(unsigned char *)(&cf->cfs1.phunit1)+3;
	*tp='\0';

	set32(cf->cfs1.phunit1,0);
	set32(cf->cfs1.phunit1,1);
	set32(cf->cfs1.phunit1,2);

	set32(cf->cfs1.phunit1,24);         //0 1

	set32(cf->cfs1.anunit1,1);
	set32(cf->cfs1.anunit1,3);

	set32(cf->cfs1.phunit2,0);
	set32(cf->cfs1.phunit2,1);
	set32(cf->cfs1.phunit2,3);

	set32(cf->cfs1.anunit2,1);
	set32(cf->cfs1.anunit2,2);

	cf->chk=PcrcFunc((unsigned char *)msg, cf->framesize-2, 0);
	unsigned short tcrc=*(unsigned short *)((unsigned char *)cf+sizeof(ConfFrame)-2);
	printf("crc=:%d,size=:%d  tcrc=%d\n",cf->chk,cf->framesize,tcrc);
}

void SendDataFrame(void * msg,int t)
{
	printf("+++++++++++SendDataFrame+++++++++++\n");
	DataFrame* df=(DataFrame*)msg;
	df->framesize=sizeof(DataFrame);
	memcpy((unsigned char*)&df->dsf1.stats,"ab",sizeof("ab"));
	switch(t)
		{
			case 0:
			{
				df->dsf1.analog1=11;
				df->dsf1.analog2=12;
				break;
			}
			case 1:
			{
				df->dsf1.analog1=33;
				df->dsf1.analog2=32;
				break;
			}
			case 2:
			{
				df->dsf1.analog1=88;
				df->dsf1.analog2=99;
				break;
			}
		}
	df->chk=PcrcFunc((unsigned char *)df, df->framesize-2, 0);
	unsigned short tcrc=*(unsigned short *)((unsigned char *)df+sizeof(DataFrame)-2);
	printf("crc=:%d,size=:%d  tcrc=%d\n",df->chk,df->framesize,tcrc);
}
void ClientMode()
{
		printf("\033[2J");
		printf("------------Client Model-------------\n");
		struct sockaddr_in saddr;
		memset(&saddr,0,sizeof(saddr));
		saddr.sin_family=AF_INET;
		saddr.sin_addr.s_addr=inet_addr((const char*)ServerIp);
		saddr.sin_port=htons(PORT);

		int cfd=socket(AF_INET,SOCK_STREAM,0);
		if(cfd==-1)
		{
			fprintf(stderr,"socket was failed:%s\n",strerror(errno));
			close(cfd);
			exit(1);
		}
		const int opt=1;
		if(setsockopt(cfd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt))==-1)
		{
			fprintf(stderr,"setsockopt SO_REUSEADDR was failed:%s\n",strerror(errno));
			close(cfd);
			exit(1);
		}
	
		int socklen=sizeof(struct sockaddr_in);
		if(connect(cfd,(struct sockaddr*)&saddr,socklen)==-1)                         //阻塞模式连接
		{
			fprintf(stderr,"connect was failed:%s\n",strerror(errno));
			close(cfd);
			exit(1);
		}
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		//非阻塞模式连接的实例：这段代码很成熟
		unsigned long opt=1;
		int waittime=5;                                          //非阻塞模式5s超时
		if(ioctl(cfd,FIONBIO,&opt)==-1)
		{
			return false;
		}
		int ret=connect(cfd,(struct sockaddr*)&saddr,socklen);
		if(ret==-1)
		{
			struct timeval tv;
			tv.tv_sec=waittime;
			tv.tv_usec=0;
			fd_set rset;
			FD_ZERO(&rset);
			FD_SET(cfd,&rset);
			ret=select(cfd+1,NULL,&rset,NULL,&tv);
			if(ret<=0)
			{
				fprintf(stderr,"连接服务器失败\n");
				close(cfd);
				cfd=-1;
				return -1;
			}
			else
			{
				ret=false;
			}
		}
		printf("连接服务器成功\n");
		return cfd;
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		//Send One Time Config Frame
		struct ConfFrame cf;
		memset(&cf,0,sizeof(cf));
		SendConfFrame(&cf,0);
		int res=send(cfd,&cf,sizeof(cf),0);
		if(res<0)
		{

		}
		//Send Data Frame
		int ctype=1;
		struct DataFrame df;
		memset(&df,0,sizeof(df));
		char buf[1024]={0};
		int i=0;
		while(1)
		{
			SendDataFrame(&df,ctype);
			res=send(cfd,&df,sizeof(df),0);
//			memset(buf,0,sizeof(buf));
//			sprintf(buf,"%d",i);
//			res=send(cfd,buf,sizeof(buf),0);
			++i;
			usleep(100000);
		}
	}
