#include "include.h"

//  �ṹ�嶨��
PID     spid;
/**********************************************************************************************************
�������ƣ�PID��ʼ��
�����������
�����������
�������أ���
����˵������
**********************************************************************************************************/
void PIDInit(void)
{
    spid.SumError   = 0;                                                //  Sums of Errors
    spid.LastError  = 0;                                                //  Error[-1]
    spid.PrevError  = 0;                                                //  Error[-2]

    spid.Proportion = 15;                                               //  �������� Proportional Const
    spid.Integral   = 5;                                                //  ���ֳ��� Integral Const
    spid.Derivative = 30;                                                //  ΢�ֳ��� Derivative Const   
    spid.SetPoint   = SETTEMPER*10;                                 //  �趨Ŀ�� Desired Value
}

/**********************************************************************************************************
�������ƣ�����ʽPID����
����������������
�����������
�������أ���
����˵�������ֵ
**********************************************************************************************************/
int IncPIDCalc(unsigned int NextPoint)
{
    int iError, iIncpid;
    
    //  ��ǰ���
    iError = spid.SetPoint - NextPoint;
    
//    if(iError>30)
//        spid.Proportion=99;
//    else
//        spid.Proportion=30;
				
    //  ��������
    iIncpid = (int)(spid.Proportion * iError                                 //  E[K]��
                  - spid.Integral * spid.LastError                          //  E[K-1]��
                  + spid.Derivative * spid.PrevError);                       //  E[K-2]��
    
    //  �洢�������´μ���
    spid.PrevError = spid.LastError;
    spid.LastError = iError;
    
    //  ��������ֵ
    
    return (iIncpid);
}

/**********************************************************************************************************
�������ƣ��¶ȱȽ�
�����������ǰ�ɼ������¶�
�����������
�������أ���
����˵�������ֵ
**********************************************************************************************************/
void TemperControl(short temp)
{        
    //  �趨�¶ȴ��ڲɼ��¶�
    if (spid.SetPoint > temp)
    {
        if(spid.SetPoint - temp > 1)//BangBang���ơ�1��
        {
            spid.PWM_value = 1000;
        }
        else
        {
            spid.PWM_value += IncPIDCalc(temp);                          //  Perform PID Interation        
        }
    }
    
    //  �趨�¶�С�ڵ��ڲɼ�ֵ
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
    
    //  �޶�ռ�ձȵ����ֵ����Сֵ
		spid.PWM_value = spid.PWM_value > 1000?1000:(spid.PWM_value <= 0?0:spid.PWM_value);

    TIM_SetCompare3(TIM3, spid.PWM_value);    
}