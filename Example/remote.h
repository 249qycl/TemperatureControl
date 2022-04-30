#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f4xx.h"

//  红外遥控识别码(ID),默认遥控器识别码为0
#define REMOTE_ID   0

struct remotedata 
{
    unsigned char  RmtSta;
    unsigned short Dval;												//	下降沿时计数器的值
    unsigned int   RmtRec;												//	红外接收到的数据
    unsigned char  RmtCnt;												//	按键按下的次数
};
typedef struct remotedata   REMOTEDATA;
typedef struct remotedata*  PREMOTEDATA;

//  红外数据输入脚
#define RDATA       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)	 


void Remote_Init(void);    //红外传感器接收头引脚初始化
unsigned char Remote_Scan(void);
void PB0_Init(void);

/********************用户程序*************************/
void key_process(void);
#endif

