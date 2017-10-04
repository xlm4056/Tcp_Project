/*
 * DataS.h
 *
 *  Created on: 2017年4月4日
 *      Author: xieliming
 */

#ifndef DATAS_H_
#define DATAS_H_
#define BUF_SIZE 100*1024
#define DATA_CNT 200
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)

struct ConfSubFrame
{
	unsigned char stn[16];                           //站名
	unsigned char idcode[8];                         //PMU硬件标识
	unsigned short format;                           //数据帧的数据格式
	unsigned short phnmr;                            //相量的数量
	unsigned short annmr;                            //模拟量数量
	unsigned short dgnmr;                            //开关量状态字的数量

	unsigned char chnam_ph[16*2];                         //##############

	unsigned char chnam_an[16*2];

	unsigned char chnam_dg[16*16*2];
	unsigned int phunit1;                             //相量通道的转换因子
	unsigned int phunit2;
	unsigned int anunit1;                             //模拟通道的转换因子
	unsigned int anunit2;
	unsigned int digunit1;
	unsigned int digunit2;
	unsigned short fnom;                             //额定频率和标志
};
struct ConfFrame
{
	unsigned short sync;
	unsigned short framesize;
	unsigned int soc;
	unsigned short  d_frame;
	unsigned int meas_rate;
	unsigned short num_pmu;                         //######################
	struct ConfSubFrame cfs1;
	unsigned short period;
	unsigned short chk;
};




struct DataSubFrame
{
	unsigned short stats;                                //状态字diagnostic message####################
	unsigned short phasorsval1;                            //
	short phasorangle1;
	unsigned short phasorsval2;                            //
	short phasorangle2;
	short freq;
	short dfreq;
	short analog1;
	short analog2;
	unsigned short digital1;
	unsigned short digital2;
};
struct DataFrame
{
	unsigned short sync;
	unsigned short framesize;                         //帧字节数
	unsigned int soc;
	unsigned int fracsec;
	struct DataSubFrame dsf1;
	unsigned short chk;
};
void clr16(unsigned short &a,int index);
void set16(unsigned short &a,int index);
void clr32(unsigned int &a,int index);
void set32(unsigned int &a,int index);
void show32(unsigned int &a,const char * msg);
void show16(unsigned short &a,const char * msg);
unsigned short PcrcFunc(unsigned char *pBuf, int length, int init_val);             //CRC16 计算
#endif /* DATAS_H_ */
