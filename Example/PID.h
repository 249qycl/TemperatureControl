#ifndef _PID_H_
#define _PID_H_

#include "stm32f4xx.h"

#define     SETTEMPER   35.0


//  PID�ṹ��
struct PID
{
    int  SetPoint;                                                       //  �趨Ŀ�� Desired Value
    long SumError;                                                      //  �ۼ����
    double Proportion;                                                  //  �������� Proportional Const
    double Integral;                                                    //  ���ֳ��� Integral Const
    double Derivative;                                                  //  ΢�ֳ��� Derivative Const
    
    int LastError;                                                      //  Error[-1]
    int PrevError;                                                      //  Error[-2]
    int PWM_value;                                                      //  PWMռ�ձ�ֵ
};
typedef struct PID    PID;
typedef struct PID*   PPID; 

//  �ṹ������
extern PID   spid;

//  ��������
void PIDInit(void);
int IncPIDCalc(unsigned int NextPoint);
void TemperControl(short temp);

#endif
