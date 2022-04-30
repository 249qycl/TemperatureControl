#include "include.h"

/**********************************************************************************************************
�������ƣ�����DS18B20����Ϊ���
�����������
�����������
�������أ���
//DS18B20         PF13	
**********************************************************************************************************/
void DS18B20IOOUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //DS18B20         PF13	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}


/**********************************************************************************************************
�������ƣ�����DS18B20����Ϊ����
�����������
�����������
�������أ���
//DS18B20         PF13	
**********************************************************************************************************/
void DS18B20IOIN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //DS18B20         PF13	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);               //  ʹ��IOʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                        //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**********************************************************************************************************
�������ƣ���ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
�����������
�����������
�������أ�1		������
		  0		����
**********************************************************************************************************/	 
unsigned char DS18B20Init(void)
{
 	DS18B20Reset();
	
	return DS18B20Check();
} 


/**********************************************************************************************************
�������ƣ���λDS18B20
�����������
�����������
�������أ���
**********************************************************************************************************/
void DS18B20Reset(void)	   
{                 
	DS18B20IOOUT();
	
	DS18B20IO_L;                                                        //  ����DQ
	delay_us(750);                                                      //  ����750us
	
    DS18B20IO_H;	
	delay_us(15);                                                       //  15US
}

/**********************************************************************************************************
�������ƣ��ȴ�DS18B20�Ļ�Ӧ
�����������
�����������
�������أ�1	δ��⵽DS18B20�Ĵ���
		  0	����
**********************************************************************************************************/
unsigned char DS18B20Check(void) 	   
{   
	unsigned char retry = 0;
	
	DS18B20IOIN();
    
	while (DS18B20READ && retry < 200)
	{
		retry++;
		delay_us(1);
	} 
	
	if(retry >= 200)
	{
		return 1;
	}
	else 
	{
		retry = 0;
	}
	
    while (!DS18B20READ && retry < 240)
	{
		retry++;
		delay_us(1);
	}
	
	if(retry >= 240)
	{
		return 1;	    
	}
	
	return 0;
}

/**********************************************************************************************************
�������ƣ���DS18B20��ȡһ��λ
�����������
�����������
�������أ�1/0
**********************************************************************************************************/
unsigned char DS18B20ReadBit(void)
{
	unsigned char data;
	
	DS18B20IOOUT();
	DS18B20IO_L;
	delay_us(10);
	
	DS18B20IO_H; 
	DS18B20IOIN();
	delay_us(12);
	
	if(DS18B20READ)
	{
		data = 1;
	}
	else 
	{
		data = 0;	 
	}
  
	delay_us(50);           
  
	return data;
}


/**********************************************************************************************************
�������ƣ���DS18B20��ȡһ���ֽ�
�����������
�����������
�������أ�����������
**********************************************************************************************************/
unsigned char DS18B20ReadByte(void)
{        
    unsigned char i, j, dat;
    dat = 0;
	
	for (i = 1; i <= 8; i++) 
	{
        j = DS18B20ReadBit();
        dat = (j << 7)|(dat >> 1);
    }						   
	
    return dat;
}


/**********************************************************************************************************
�������ƣ�дһ���ֽڵ�DS18B20
���������д�������
�����������
�������أ���
**********************************************************************************************************/
void DS18B20WriteByte(unsigned char dat)     
 {             
    unsigned char j;
    unsigned char testb;
	
	DS18B20IOOUT();
     
    for(j = 1; j <= 8; j++) 
	{
        testb = dat&0x01;
        dat   = dat >> 1;
        if (testb) 
        {
            DS18B20IO_L;
            delay_us(2);   
			
            DS18B20IO_H;
            delay_us(60);             
        }
        else 
        {
            DS18B20IO_L;
            delay_us(60);             
            
			DS18B20IO_H;
            delay_us(2);                          
        }
    }
}

/**********************************************************************************************************
�������ƣ���ʼ�¶�ת��
�����������
�����������
�������أ���
**********************************************************************************************************/
void DS18B20Start(void)
{   						               
    DS18B20Reset();	   
	DS18B20Check();	 
    DS18B20WriteByte(0xCC);												// 	skip rom
    DS18B20WriteByte(0x44);												// 	convert
} 


/**********************************************************************************************************
�������ƣ���DS18B20�õ��¶�ֵ������0.1��
�����������
�����������
�������أ��¶�ֵ ��-550~1250��
**********************************************************************************************************/
int DS18B20GetTemp(void)
{
    unsigned char temp;
    unsigned char TL,TH;
	int tem;
	
    DS18B20Start();                    									// 	ds1820 start convert
    DS18B20Reset();
    DS18B20Check();	 
    DS18B20WriteByte(0xCC);												// 	skip rom
    DS18B20WriteByte(0xBE);												// 	convert	    
    
    TL = DS18B20ReadByte(); 											// 	LSB   
    TH = DS18B20ReadByte(); 											// 	MSB   
	
    if(TH > 7)
    {
        TH = ~TH;
        TL = ~TL; 
        temp = 0;														//	�¶�Ϊ��  
    }
	else 
	{
		temp = 1;														//	�¶�Ϊ��	  	  
	}
	
    tem = TH; 															//	��ø߰�λ
    
	tem <<= 8;    
    tem += TL;															//	��õװ�λ
    tem = (int)((double)tem*6.25);											//	ת��     
	
	if(temp)
	{
		return tem; 													//	�����¶�ֵ
	}
	else 
	{
		return -tem;    
	}
}

/************************�û�����**********************************/
void ds18b20_init_show()
{  
#define INIT_SEAT_X 60
#define INIT_SEAT_Y (-30)
    while(DS18B20Init())	                                            //  DS18B20��ʼ��
    {
        LCD_ShowString(10, 160, "DS18B20��ʼ��ʧ��",YELLOW, BLACK);
		delay_ms(500);
    }  		
    LCD_ShowString(INIT_SEAT_X+115, 100+INIT_SEAT_Y, "DS18B20��ʼ�����",YELLOW, BLACK);
	delay_ms(500);                   
	LCD_ShowString(INIT_SEAT_X+120, 120+INIT_SEAT_Y, "EEPROM��ʼ�����",YELLOW, BLACK);
	delay_ms(500);                   
	LCD_ShowString(INIT_SEAT_X+110, 140+INIT_SEAT_Y, "����ң�س�ʼ�����",YELLOW, BLACK);
	delay_ms(500);                   
	LCD_ShowString(INIT_SEAT_X+110, 160+INIT_SEAT_Y, "����ͨ�ų�ʼ�����",YELLOW, BLACK);
	delay_ms(500);                   
	LCD_ShowString(INIT_SEAT_X+130, 180+INIT_SEAT_Y, "������ʼ�����",YELLOW, BLACK);
	delay_ms(500);                   
	LCD_ShowString(INIT_SEAT_X+140, 200+INIT_SEAT_Y, "LED��ʼ�����",YELLOW, BLACK);
	delay_ms(500);
}
int temperature=350;
float Alarm_value=35.0;
void Alarm_temp(float temp)
{
	static uint8 warn_flag=0;//,turn_flag=0;
	warn_flag=temp>(Alarm_value+0.5)?1:temp<(Alarm_value-0.5)?1:0;
	if(warn_flag==1)
	{
		LED1_REVERSE;
		LED2_REVERSE;
		LED3_REVERSE;
		LED4_REVERSE;
		//turn_flag=!turn_flag;
	}
	else
	{
		LED1_OFF;
		LED2_OFF;
		LED3_OFF;
		LED4_OFF;
	}
}

void Usual_data_show()
{
	static u8 times1=0;
	
	if(times1++==2)
	{
		times1=0;
		Alarm_temp((float)temperature/100);
		Vitta_cursor_show((float)temperature/100);
	}	
	TFT_INPUT("�¶�:",temperature/100,0);
	TFT_INPUT("ƫ��:",spid.SetPoint-(temperature/10),1);
	if(save_flag>0)
	{
		save_flag++;
		LCD_Show(220, 150,RED,  BLACK,"����ɹ���");	
		if(save_flag==25)
		{
			save_flag=0;
			LCD_Show(220, 150,BLACK,BLACK,"����ɹ���");
		}
	}
}