#ifndef _PID_H_
#define _PID_H_

#include "stm32f4xx.h"

#define     SETTEMPER   35.0


//  PID结构体
struct PID
{
    int  SetPoint;                                                       //  设定目标 Desired Value
    long SumError;                                                      //  累计误差
    double Proportion;                                                  //  比例常数 Proportional Const
    double Integral;                                                    //  积分常数 Integral Const
    double Derivative;                                                  //  微分常数 Derivative Const
    
    int LastError;                                                      //  Error[-1]
    int PrevError;                                                      //  Error[-2]
    int PWM_value;                                                      //  PWM占空比值
};
typedef struct PID    PID;
typedef struct PID*   PPID; 

//  结构体声明
extern PID   spid;

//  函数声明
void PIDInit(void);
int IncPIDCalc(unsigned int NextPoint);
void TemperControl(short temp);

#endif
