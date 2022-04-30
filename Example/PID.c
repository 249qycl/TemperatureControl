#include "include.h"

//  结构体定义
PID     spid;
/**********************************************************************************************************
函数名称：PID初始化
输入参数：无
输出参数：无
函数返回：无
函数说明：无
**********************************************************************************************************/
void PIDInit(void)
{
    spid.SumError   = 0;                                                //  Sums of Errors
    spid.LastError  = 0;                                                //  Error[-1]
    spid.PrevError  = 0;                                                //  Error[-2]

    spid.Proportion = 15;                                               //  比例常数 Proportional Const
    spid.Integral   = 5;                                                //  积分常数 Integral Const
    spid.Derivative = 30;                                                //  微分常数 Derivative Const   
    spid.SetPoint   = SETTEMPER*10;                                 //  设定目标 Desired Value
}

/**********************************************************************************************************
函数名称：增量式PID计算
输入参数：输入参数
输出参数：无
函数返回：无
函数说明：输出值
**********************************************************************************************************/
int IncPIDCalc(unsigned int NextPoint)
{
    int iError, iIncpid;
    
    //  当前误差
    iError = spid.SetPoint - NextPoint;
    
//    if(iError>30)
//        spid.Proportion=99;
//    else
//        spid.Proportion=30;
				
    //  增量计算
    iIncpid = (int)(spid.Proportion * iError                                 //  E[K]项
                  - spid.Integral * spid.LastError                          //  E[K-1]项
                  + spid.Derivative * spid.PrevError);                       //  E[K-2]项
    
    //  存储误差，用于下次计算
    spid.PrevError = spid.LastError;
    spid.LastError = iError;
    
    //  返回增量值
    
    return (iIncpid);
}

/**********************************************************************************************************
函数名称：温度比较
输入参数：当前采集到的温度
输出参数：无
函数返回：无
函数说明：输出值
**********************************************************************************************************/
void TemperControl(short temp)
{        
    //  设定温度大于采集温度
    if (spid.SetPoint > temp)
    {
        if(spid.SetPoint - temp > 1)//BangBang控制【1】
        {
            spid.PWM_value = 1000;
        }
        else
        {
            spid.PWM_value += IncPIDCalc(temp);                          //  Perform PID Interation        
        }
    }
    
    //  设定温度小于等于采集值
    else if(spid.SetPoint <= temp)
    {
        if(temp - spid.SetPoint > 1)
        {
            spid.PWM_value = 0;
        }
        else
        {
            spid.PWM_value += IncPIDCalc(temp);                          //  Perform PID Interation
        }
    }
    
    //  限定占空比的最大值和最小值
		spid.PWM_value = spid.PWM_value > 1000?1000:(spid.PWM_value <= 0?0:spid.PWM_value);

    TIM_SetCompare3(TIM3, spid.PWM_value);    
}