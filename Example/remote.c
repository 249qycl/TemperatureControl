#include "include.h"

REMOTEDATA remote;

/**********************************************************************************************************
函数名称：红外遥控初始化，设置IO以及TIM2_CH1的输入捕获
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
void Remote_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM1_ICInitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //  使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //	TIM1时钟使能

    //	GPIOA8  复用功能,上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //	复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //	推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //	上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);             //	初始化

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1); //	GPIOA8复用为TIM1

    TIM_TimeBaseStructure.TIM_Prescaler = 167;                  //	预分频器,1M的计数频率,1us加1.
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //	向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 10000;                   //	设定计数器自动重装值 最大10ms溢出
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    //	初始化TIM2输入捕获参数
    TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1;                //	CC1S=01 	选择输入端 IC1映射到TI1上
    TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     //	上升沿捕获
    TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //	映射到TI1上
    TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //	配置输入分频,不分频
    TIM1_ICInitStructure.TIM_ICFilter = 0x03;                        //	IC1F=0003 8个定时器时钟周期滤波
    TIM_ICInit(TIM1, &TIM1_ICInitStructure);                         //	初始化定时器2输入捕获通道
    TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE);          //	允许更新中断 ,允许CC1IE捕获中断
    TIM_Cmd(TIM1, ENABLE);                                           //	使能定时器1

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //	抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //	子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //	IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //	初始化NVIC寄存器

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //	抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //	子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //	IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //	初始化NVIC寄存器
}

//unsigned char  RmtSta = 0;
//unsigned short Dval;													//	下降沿时计数器的值
//unsigned int   RmtRec = 0;												//	红外接收到的数据
//unsigned char  RmtCnt = 0;												//	按键按下的次数
/**********************************************************************************************************
函数名称：遥控器接收状态中断
输入参数：无
输出参数：无
函数返回：无
参数说明：
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留
//[4]:标记上升沿是否已经被捕获
//[3:0]:溢出计时器
**********************************************************************************************************/
void TIM1_UP_TIM10_IRQHandler(void)
{

    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) //	溢出中断
    {
        if (remote.RmtSta & 0x80) //	上次有数据被接收到了
        {
            remote.RmtSta &= ~0X10; //	取消上升沿已经被捕获标记
            if ((remote.RmtSta & 0X0F) == 0X00)
            {
                remote.RmtSta |= 1 << 6; //	标记已经完成一次按键的键值信息采集
            }
            if ((remote.RmtSta & 0X0F) < 14)
            {
                remote.RmtSta++;
            }
            else
            {
                remote.RmtSta &= ~(1 << 7); //	清空引导标识
                remote.RmtSta &= 0XF0;      //	清空计数器
            }
        }
    }

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //	清除中断标志位
}

/**********************************************************************************************************
函数名称：定时器1输入捕获中断服务程序
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
void TIM1_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET) //	处理捕获(CC1IE)中断
    {
        if (RDATA) //	上升沿捕获
        {
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Falling); //	CC1P=1 设置为下降沿捕获
            TIM_SetCounter(TIM1, 0);                             //	清空定时器值
            remote.RmtSta |= 0X10;                               //	标记上升沿已经被捕获
        }
        else //	下降沿捕获
        {
            remote.Dval = TIM_GetCapture1(TIM1);                //	读取CCR1也可以清CC1IF标志位
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Rising); //	CC1P=0	设置为上升沿捕获

            if (remote.RmtSta & 0X10) //	完成一次高电平捕获
            {
                if (remote.RmtSta & 0X80) //	接收到了引导码
                {

                    if (remote.Dval > 300 && remote.Dval < 800) //	560为标准值,560us
                    {
                        remote.RmtRec <<= 1; //	左移一位.
                        remote.RmtRec |= 0;  //	接收到0（因为初始化是0，所以不变）
                    }
                    else if (remote.Dval > 1400 && remote.Dval < 1800) //	1680为标准值,1680us
                    {
                        remote.RmtRec <<= 1; //	左移一位.
                        remote.RmtRec |= 1;  //	接收到1
                    }
                    else if (remote.Dval > 2200 && remote.Dval < 2600) //	得到按键键值增加的信息 2500为标准值2.5ms
                    {
                        remote.RmtCnt++;       //	按键次数增加1次
                        remote.RmtSta &= 0XF0; //	清空计时器
                    }
                }
                else if (remote.Dval > 4200 && remote.Dval < 4700) //	4500为标准值4.5ms
                {
                    remote.RmtSta |= 1 << 7; //	标记成功接收到了引导码
                    remote.RmtCnt = 0;       //	清除按键次数计数器
                }
            }
            remote.RmtSta &= ~(1 << 4);
        }
    }

    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1); //	清除中断标志位
}

/**********************************************************************************************************
函数名称：红外按键扫描
输入参数：无
输出参数：无
函数返回：0		没有任何按键按下	
		  其他	按下的按键键值.
**********************************************************************************************************/
unsigned char Remote_Scan(void)
{
    unsigned char sta = 0;
    unsigned char t1, t2;

    if (remote.RmtSta & (1 << 6)) //	得到一个按键的所有信息了
    {
        t1 = remote.RmtRec >> 24;                          //	得到地址码
        t2 = (remote.RmtRec >> 16) & 0xff;                 //	得到地址反码
        if ((t1 == (unsigned char)~t2) && t1 == REMOTE_ID) //	检验遥控识别码(ID)及地址
        {
            t1 = remote.RmtRec >> 8;
            t2 = remote.RmtRec;
            if (t1 == (unsigned char)~t2)
            {
                sta = t1; //	键值正确
            }
        }
        if ((sta == 0) || ((remote.RmtSta & 0X80) == 0)) //	按键数据错误/遥控已经没有按下了
        {
            remote.RmtSta &= ~(1 << 6); //	清除接收到有效按键标识
            remote.RmtCnt = 0;          //	清除按键次数计数器
        }
    }
    return sta;
}

/********************用户程序*************************/
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
//按键处理
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
        LCD_Show(160, 200 + i * 20, Color[seat[i + 3]], BLACK, "状态:%c", "OI"[Scope.state[i]]);
    }
    LCD_Show(X_SEAT - 70, Y_SEAT + 180, Color[seat[10]], BLACK, "设定温度:%2.1f度", (float)spid.SetPoint / 10);
    LCD_Show(X_SEAT - 70, Y_SEAT + 200, Color[seat[11]], BLACK, "P:%2.1f", spid.Proportion);
    LCD_Show(X_SEAT - 70, Y_SEAT + 220, Color[seat[12]], BLACK, "I:%2.1f", spid.Integral);
    LCD_Show(X_SEAT - 70, Y_SEAT + 240, Color[seat[13]], BLACK, "D:%2.1f", spid.Derivative);
    LCD_Show(X_SEAT - 10, Y_SEAT + 200, Color[seat[14]], BLACK, "报警温度:%2d", (int)Alarm_value);

    LCD_Show(220, 180, Color[seat[6]], BLACK, "光标:%3d", Scope.cursor_seat);
    LCD_Show(220, 200, Color[seat[7]], BLACK, "STEP:%2d", Scope.step);
    LCD_Show(220, 220, Color[seat[8]], BLACK, "启停:%c", "OI"[Scope.stop]);
    LCD_Show(220, 240, Color[seat[9]], BLACK, "AUTO:%c", "OI"[Scope.auto_switch]);
}
