#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f4xx.h"

//  ����ң��ʶ����(ID),Ĭ��ң����ʶ����Ϊ0
#define REMOTE_ID   0

struct remotedata 
{
    unsigned char  RmtSta;
    unsigned short Dval;												//	�½���ʱ��������ֵ
    unsigned int   RmtRec;												//	������յ�������
    unsigned char  RmtCnt;												//	�������µĴ���
};
typedef struct remotedata   REMOTEDATA;
typedef struct remotedata*  PREMOTEDATA;

//  �������������
#define RDATA       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)	 


void Remote_Init(void);    //���⴫��������ͷ���ų�ʼ��
unsigned char Remote_Scan(void);
void PB0_Init(void);

/********************�û�����*************************/
void key_process(void);
#endif

