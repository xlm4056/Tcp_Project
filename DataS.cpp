/*
 * DataS.c
 *
 *  Created on: 2017年4月4日
 *      Author: xieliming
 */
#include "DataS.h"
void set32(unsigned int &a,int index)
{
	a|=(1<<index);
}
void clr32(unsigned int &a,int index)
{
	a&=~(1<<index);
}
void clr16(unsigned short &a,int index)
{
	a&=~(1<<index);
}
void set16(unsigned short &a,int index)
{
	a|=(1<<index);
}
void show16(unsigned short &a,const char * msg)
{
	printf("%s ",msg);
	printf("%d\n",a);
	for(int i=15;i>=0;--i)
	{
		if(i==23||i==15||i==7)
		{
			printf("  ");
		}
		if(a&(1<<i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
	printf("\n");
}
void show32(unsigned int &a,const char * msg)
{
	printf("%s ",msg);
	printf("%d\n",a);
	for(int i=31;i>=0;--i)
	{
		if(i==23||i==15||i==7)
		{
			printf("  ");
		}
		if(a&(1<<i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
	printf("\n");
}
unsigned short PcrcFunc(unsigned char *pBuf, int length, int init_val)    //init_val default 0
{
	unsigned long Temp;
	unsigned long CRC_Code = init_val;
	int   i,j;
	for( i=0; i<length; i++)
    	{
		Temp  =  pBuf[i] << 8;
		CRC_Code  =  CRC_Code ^ Temp;
		for ( j=0; j<8; j++)
		{
			CRC_Code  =  CRC_Code & 0xFFFF;
			CRC_Code  =  CRC_Code << 1;
			if( CRC_Code > 0xFFFF)
			{
				CRC_Code = CRC_Code ^ 0x1021;
			}
		}
  	}
	CRC_Code = CRC_Code & 0xFFFF;
	return CRC_Code;
}
