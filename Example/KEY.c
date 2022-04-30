#include "include.h"

/******************************************************************************************
*�������ƣ�void KEYGpio_Init(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����KEY��ʼ��
//KEY_S1      PB2
//KEY_S2      PA0
//KEY_S3      PC2
//KEY_S4      PC0
//KEY_S5      PE4
*******************************************************************************************/
void KEYGpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //  KEY_S1      PB2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //  ʹ��IOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //  ʹ��SYSCFGʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //  �����Ͻ�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //  IO��Ϊ0
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //  KEY_S2      PA0
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //  ʹ��IOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //  ʹ��SYSCFGʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //  �����Ͻ�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //  KEY_S3 PC2, KEY_S4  PC0
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //  ʹ��IOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //  ʹ��SYSCFGʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //  �����Ͻ�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //KEY_S5      PE4
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //  ʹ��IOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //  ʹ��SYSCFGʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //  �����Ͻ�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

uint8 key_read()
{
    static uint8 times = 0, re_flag = 0;
    uint8 value;

    if (KEY_S1_READ == 0)
        value = 1;
    else if (KEY_S2_READ == 0)
        value = 2;
    else if (KEY_S3_READ == 0)
        value = 3;
    else if (KEY_S4_READ == 0)
        value = 4;
    else if (KEY_S5_READ == 0)
        value = 5;
    else
        value = 0;
    times = value > 0 ? (times + 1) : 0;
    if (times > 2)
    {
        if (re_flag == 0)
        {
            re_flag = 1;
            return value;
        }
    }
    else
        re_flag = 0;

    return 0xff;
}