#include "include.h"

REMOTEDATA remote;

/**********************************************************************************************************
�������ƣ�����ң�س�ʼ��������IO�Լ�TIM2_CH1�����벶��
�����������
�����������
�������أ���
**********************************************************************************************************/
void Remote_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM1_ICInitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //  ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //	TIM1ʱ��ʹ��

    //	GPIOA8  ���ù���,����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //	���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //	�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //	����
    GPIO_Init(GPIOA, &GPIO_InitStructure);             //	��ʼ��

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1); //	GPIOA8����ΪTIM1

    TIM_TimeBaseStructure.TIM_Prescaler = 167;                  //	Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //	���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period = 10000;                   //	�趨�������Զ���װֵ ���10ms���
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    //	��ʼ��TIM2���벶�����
    TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1;                //	CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
    TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     //	�����ز���
    TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //	ӳ�䵽TI1��
    TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //	���������Ƶ,����Ƶ
    TIM1_ICInitStructure.TIM_ICFilter = 0x03;                        //	IC1F=0003 8����ʱ��ʱ�������˲�
    TIM_ICInit(TIM1, &TIM1_ICInitStructure);                         //	��ʼ����ʱ��2���벶��ͨ��
    TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE);          //	��������ж� ,����CC1IE�����ж�
    TIM_Cmd(TIM1, ENABLE);                                           //	ʹ�ܶ�ʱ��1

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //	��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //	�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //	IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //	��ʼ��NVIC�Ĵ���

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //	��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //	�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //	IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //	��ʼ��NVIC�Ĵ���
}

//unsigned char  RmtSta = 0;
//unsigned short Dval;													//	�½���ʱ��������ֵ
//unsigned int   RmtRec = 0;												//	������յ�������
//unsigned char  RmtCnt = 0;												//	�������µĴ���
/**********************************************************************************************************
�������ƣ�ң��������״̬�ж�
�����������
�����������
�������أ���
����˵����
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����
//[4]:����������Ƿ��Ѿ�������
//[3:0]:�����ʱ��
**********************************************************************************************************/
void TIM1_UP_TIM10_IRQHandler(void)
{

    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) //	����ж�
    {
        if (remote.RmtSta & 0x80) //	�ϴ������ݱ����յ���
        {
            remote.RmtSta &= ~0X10; //	ȡ���������Ѿ���������
            if ((remote.RmtSta & 0X0F) == 0X00)
            {
                remote.RmtSta |= 1 << 6; //	����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
            }
            if ((remote.RmtSta & 0X0F) < 14)
            {
                remote.RmtSta++;
            }
            else
            {
                remote.RmtSta &= ~(1 << 7); //	���������ʶ
                remote.RmtSta &= 0XF0;      //	��ռ�����
            }
        }
    }

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //	����жϱ�־λ
}

/**********************************************************************************************************
�������ƣ���ʱ��1���벶���жϷ������
�����������
�����������
�������أ���
**********************************************************************************************************/
void TIM1_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET) //	������(CC1IE)�ж�
    {
        if (RDATA) //	�����ز���
        {
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Falling); //	CC1P=1 ����Ϊ�½��ز���
            TIM_SetCounter(TIM1, 0);                             //	��ն�ʱ��ֵ
            remote.RmtSta |= 0X10;                               //	����������Ѿ�������
        }
        else //	�½��ز���
        {
            remote.Dval = TIM_GetCapture1(TIM1);                //	��ȡCCR1Ҳ������CC1IF��־λ
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Rising); //	CC1P=0	����Ϊ�����ز���

            if (remote.RmtSta & 0X10) //	���һ�θߵ�ƽ����
            {
                if (remote.RmtSta & 0X80) //	���յ���������
                {

                    if (remote.Dval > 300 && remote.Dval < 800) //	560Ϊ��׼ֵ,560us
                    {
                        remote.RmtRec <<= 1; //	����һλ.
                        remote.RmtRec |= 0;  //	���յ�0����Ϊ��ʼ����0�����Բ��䣩
                    }
                    else if (remote.Dval > 1400 && remote.Dval < 1800) //	1680Ϊ��׼ֵ,1680us
                    {
                        remote.RmtRec <<= 1; //	����һλ.
                        remote.RmtRec |= 1;  //	���յ�1
                    }
                    else if (remote.Dval > 2200 && remote.Dval < 2600) //	�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
                    {
                        remote.RmtCnt++;       //	������������1��
                        remote.RmtSta &= 0XF0; //	��ռ�ʱ��
                    }
                }
                else if (remote.Dval > 4200 && remote.Dval < 4700) //	4500Ϊ��׼ֵ4.5ms
                {
                    remote.RmtSta |= 1 << 7; //	��ǳɹ����յ���������
                    remote.RmtCnt = 0;       //	�����������������
                }
            }
            remote.RmtSta &= ~(1 << 4);
        }
    }

    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1); //	����жϱ�־λ
}

/**********************************************************************************************************
�������ƣ����ⰴ��ɨ��
�����������
�����������
�������أ�0		û���κΰ�������	
		  ����	���µİ�����ֵ.
**********************************************************************************************************/
unsigned char Remote_Scan(void)
{
    unsigned char sta = 0;
    unsigned char t1, t2;

    if (remote.RmtSta & (1 << 6)) //	�õ�һ��������������Ϣ��
    {
        t1 = remote.RmtRec >> 24;                          //	�õ���ַ��
        t2 = (remote.RmtRec >> 16) & 0xff;                 //	�õ���ַ����
        if ((t1 == (unsigned char)~t2) && t1 == REMOTE_ID) //	����ң��ʶ����(ID)����ַ
        {
            t1 = remote.RmtRec >> 8;
            t2 = remote.RmtRec;
            if (t1 == (unsigned char)~t2)
            {
                sta = t1; //	��ֵ��ȷ
            }
        }
        if ((sta == 0) || ((remote.RmtSta & 0X80) == 0)) //	�������ݴ���/ң���Ѿ�û�а�����
        {
            remote.RmtSta &= ~(1 << 6); //	������յ���Ч������ʶ
            remote.RmtCnt = 0;          //	�����������������
        }
    }
    return sta;
}

/********************�û�����*************************/
static u8 remote_key_read(void)
{
    static u8 re_flag = 0;
    u8 value;
    value = Remote_Scan();
    if (re_flag == 0 && value != 0)
    {
        re_flag = 1;
        return value;
    }
    else if (value == 0)
    {
        re_flag = 0;
    }
    return 0xff;
}

u8 seat[20];
s16 Color[3] = {YELLOW, GREEN, RED};
//��������
void key_process(void)
{
    static u8 lock = 0;
    static char grade = 0;
    u8 remote_value=0, key_value=0, i;
    int temp = 0;

    remote_value = remote_key_read();
    if (remote_value)
    {
        switch (remote_value)
        {
        case 162:
            break; //1
        case 98:
            break; //2
        case 226:
            break; //3
        case 34:
            break; //4
        case 2:
            break; //5
        case 194:
            break; //6
        case 224:
            break; //7
        case 168:
            break; //8
        case 144:
            break; //9
        case 104:
            break; //*
        case 152:
            break; //0
        case 176:
            save_flag = 1;
            save_user_data();
            break; //#
        case 24:
            if (lock == 1)
                temp++;
            else
                grade--;
            break; //up

        case 74:
            if (lock == 1)
                temp--;
            else
                grade++;
            break; //down

        case 16:
            break; //left
        case 90:
            break; //right
        case 56:
            lock = !lock;
            break; //ok
        default:
            break;
        }
    }
    key_value = key_read();
    switch (key_value)
    {
    case 5:
        if (lock == 1)
            temp++;
        else
            grade--;
        break; //up
    case 4:
        if (lock == 1)
            temp--;
        else
            grade++;
        break; //down
    case 3:
        break;
    case 2:
        break;
    case 1:
        lock = !lock;
        break; //ok
    default:
        break;
    }

    grade = grade <= 0 ? 0 : grade;
    if (lock == 1)
    {
        switch (grade)
        {
        case 0:
            Scope.divide[0] += temp * 20;
            break;
        case 1:
            Scope.divide[1] += temp * 20;
            break;
        case 2:
            Scope.divide[2] += temp * 20;
            break;
        case 3:
            Scope.state[0] = temp != 0 ? (!Scope.state[0]) : Scope.state[0];
            break;
        case 4:
            Scope.state[1] = temp != 0 ? (!Scope.state[1]) : Scope.state[1];
            break;
        case 5:
            Scope.state[2] = temp != 0 ? (!Scope.state[2]) : Scope.state[2];
            break;
        case 6:
            Scope.cursor_seat += temp * Scope.step;
            break;
        case 7:
            Scope.step += temp;
            break;
        case 8:
            Scope.stop = temp != 0 ? (!Scope.stop) : Scope.stop;
            break;
        case 9:
            Scope.auto_switch = temp != 0 ? (!Scope.auto_switch) : Scope.auto_switch;
            break;
        case 10:
            spid.SetPoint += temp * 10;
            break;
        case 11:
            spid.Proportion += temp;
            break;
        case 12:
            spid.Integral += temp;
            break;
        case 13:
            spid.Derivative += temp;
            break;
        case 14:
            Alarm_value += temp;
            break;
        }
    }

    for (i = 0; i < 20; i++)
    {
        seat[i] = 0;
        if (i == grade)
        {
            seat[i] = 1;
            if (lock == 1)
                seat[i] = 2;
        }
    }
    for (i = 0; i < 3; i++)
    {
        LCD_Show(80, 200 + i * 20, Color[seat[i]], BLACK, "DIV:%3d", Scope.divide[i]);
        LCD_Show(160, 200 + i * 20, Color[seat[i + 3]], BLACK, "״̬:%c", "OI"[Scope.state[i]]);
    }
    LCD_Show(X_SEAT - 70, Y_SEAT + 180, Color[seat[10]], BLACK, "�趨�¶�:%2.1f��", (float)spid.SetPoint / 10);
    LCD_Show(X_SEAT - 70, Y_SEAT + 200, Color[seat[11]], BLACK, "P:%2.1f", spid.Proportion);
    LCD_Show(X_SEAT - 70, Y_SEAT + 220, Color[seat[12]], BLACK, "I:%2.1f", spid.Integral);
    LCD_Show(X_SEAT - 70, Y_SEAT + 240, Color[seat[13]], BLACK, "D:%2.1f", spid.Derivative);
    LCD_Show(X_SEAT - 10, Y_SEAT + 200, Color[seat[14]], BLACK, "�����¶�:%2d", (int)Alarm_value);

    LCD_Show(220, 180, Color[seat[6]], BLACK, "���:%3d", Scope.cursor_seat);
    LCD_Show(220, 200, Color[seat[7]], BLACK, "STEP:%2d", Scope.step);
    LCD_Show(220, 220, Color[seat[8]], BLACK, "��ͣ:%c", "OI"[Scope.stop]);
    LCD_Show(220, 240, Color[seat[9]], BLACK, "AUTO:%c", "OI"[Scope.auto_switch]);
}
