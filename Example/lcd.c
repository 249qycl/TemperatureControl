#include "LCD.h"
#include "Hzk16song.h"//���ֿ�
#include "font.h"

//�ַ�������
char strbuff[50];

//	LCD�ṹ�壬Ĭ��Ϊ����
lcd_dev lcddev;

/**********************************************************************************************************
�������ƣ�Һ��us��ʱ����
���������ʱ��
�����������
**********************************************************************************************************/
void LCD_delayus(unsigned int time)
{  
	unsigned short i=0; 
	
	while(time--)
	{
		i = 8;  
		while(i--) ;    
	}
}

/**********************************************************************************************************
�������ƣ�LCD��ʱ��������Լ1ms��
���������ʱ��
�����������
�������أ���
**********************************************************************************************************/
void LCD_delayms(unsigned int cnt)
{
	volatile unsigned int dl;
	
	while(cnt--)
	{
		for(dl = 0; dl < 20000; dl++);
	}
}


/**********************************************************************************************************
�������ƣ�д�Ĵ�������
����������Ĵ���ֵ
�����������
�������أ���
**********************************************************************************************************/
void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG = regval; //д��Ҫд�ļĴ������
}

/**********************************************************************************************************
�������ƣ�д���ݺ���
�������������
�����������
�������أ���
**********************************************************************************************************/
void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM = data;
}


/**********************************************************************************************************
�������ƣ������ݺ���
�����������
�����������
�������أ�������ֵ
**********************************************************************************************************/
u16 LCD_RD_DATA(void)
{
    vu16 ram;															//	��ֹ���Ż�
	
    ram = LCD->LCD_RAM;
    
	return ram;
}


/**********************************************************************************************************
�������ƣ�д�Ĵ������ݺ���
����������Ĵ���ֵ������
�����������
�������أ���
**********************************************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;												//	д��Ҫд�ļĴ������
    LCD->LCD_RAM = LCD_RegValue;										//	д������
}

/**********************************************************************************************************
�������ƣ����Ĵ������ݺ���
����������Ĵ���ֵ
�����������
�������أ�����������
**********************************************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);												//	д��Ҫ���ļĴ������
    LCD_delayus(5);
	
    return LCD_RD_DATA();												//	���ض�����ֵ
}

/**********************************************************************************************************
�������ƣ�дGRAM�����
�����������
�����������
�������أ���
**********************************************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}


/**********************************************************************************************************
�������ƣ�дRAM���ݺ���
�����������ɫֵ
�����������
�������أ���
**********************************************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;											//	дʮ��λGRAM
}


/**********************************************************************************************************
�������ƣ�RGBת������
���������GBR��ʽ����ɫֵ
�����������
�������أ�RGB��ʽ����ɫֵ
����˵������ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
**********************************************************************************************************/
u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;
	
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    
	rgb = (b << 11) + (g << 5) + (r << 0);
    
	return(rgb);
}


/**********************************************************************************************************
�������ƣ���ʱ����
���������ʱ��
�����������
�������أ���
**********************************************************************************************************/
void opt_delay(u8 i)
{
    while(i--);
}


/**********************************************************************************************************
�������ƣ���ȡ��ĳ�����ɫֵ
���������x,y:����
�����������
�������أ��˵����ɫ
**********************************************************************************************************/
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 r = 0;
	
    if(x >= lcddev.width || y >= lcddev.height)
    {
        return 0;														//	�����˷�Χ,ֱ�ӷ���
    }
    
    LCD_SetCursor(x, y);
    
    LCD_WR_REG(0X2E); 												    //	���Ͷ�GRAMָ��
    
    r = LCD_RD_DATA();													//	dummy Read
    
    return r;
}


/**********************************************************************************************************
�������ƣ����ù��λ��
���������Xpos:������
		  Ypos:������
�����������
�������أ���
**********************************************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if(lcddev.dir == 0) 											    //	x������Ҫ�任
    {
        Xpos = lcddev.width - 1 - Xpos;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    }
    
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

/**********************************************************************************************************
�������ƣ�����LCD���Զ�ɨ�跽��
�������������
�����������
�������أ���
**********************************************************************************************************/
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
	
	//	����ʱ��1963���ı�ɨ�跽������ʱ1963�ı䷽��
    if(lcddev.dir == 0) 
    {
        switch(dir)														//	����ת��
        {
			case 	0:
					dir = 6;
					break;
					
			case 	1:
					dir = 7;
					break;
					
			case 	2:
					dir = 4;
					break;
					
			case 	3:
					dir = 5;
					break;
					
			case 	4:
					dir = 1;
					break;
					
			case 	5:
					dir = 0;
					break;
					
			case 	6:
					dir = 3;
					break;
					
			case 	7:
					dir = 2;
					break;
        }
    }
	
	//	1963,���⴦��
    switch(dir)
    {
        case 	L2R_U2D:											//	������,���ϵ���
                regval |= (0 << 7) | (0 << 6) | (0 << 5);
                break;
                
        case 	L2R_D2U:											//	������,���µ���
                regval |= (1 << 7) | (0 << 6) | (0 << 5);
                break;
                
        case 	R2L_U2D:											//	���ҵ���,���ϵ���
                regval |= (0 << 7) | (1 << 6) | (0 << 5);
                break;
                
        case 	R2L_D2U:											//	���ҵ���,���µ���
                regval |= (1 << 7) | (1 << 6) | (0 << 5);
                break;
                
        case 	U2D_L2R:											//	���ϵ���,������
                regval |= (0 << 7) | (0 << 6) | (1 << 5);
                break;
                
        case 	U2D_R2L:											//	���ϵ���,���ҵ���
                regval |= (0 << 7) | (1 << 6) | (1 << 5);
                break;
                
        case 	D2U_L2R:											//	���µ���,������
                regval |= (1 << 7) | (0 << 6) | (1 << 5);
                break;
                
        case 	D2U_R2L:											//	���µ���,���ҵ���
                regval |= (1 << 7) | (1 << 6) | (1 << 5);
                break;
    }
    
    dirreg = 0X36;       
    LCD_WriteReg(dirreg, regval);
    
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

/**********************************************************************************************************
�������ƣ�����
���������x,y	����
		  color ��ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_SetCursor(x, y);												//	���ù��λ��
    LCD_WriteRAM_Prepare();												//	��ʼд��GRAM
    LCD->LCD_RAM = color;
}

/**********************************************************************************************************
�������ƣ����ٻ���
���������x,y:	����
		  color ��ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    if(lcddev.dir == 0)
    {
        x = lcddev.width - 1 - x;
    }
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
    
    LCD->LCD_REG = lcddev.wramcmd;
    LCD->LCD_RAM = color;
}

//����LCD��ʾ����
//dir:0,������1,����
/**********************************************************************************************************
�������ƣ�����LCD��ʾ����
���������dir:0,������1,����
�����������
�������أ���
**********************************************************************************************************/
void LCD_Display_Dir(u8 dir)
{
    if(dir == 0)														//	����
    {
        lcddev.dir = 0;													//	����
        lcddev.wramcmd = 0X2C;										    //	����д��GRAM��ָ��
        lcddev.setxcmd = 0X2B;										    //	����дX����ָ��
        lcddev.setycmd = 0X2A;										    //	����дY����ָ��
        lcddev.width = 272;											    //	���ÿ��272
        lcddev.height = 480;										    //	���ø߶�480
    }
    else 																//	����
    {
        lcddev.dir = 1;													//	����
        lcddev.wramcmd = 0X2C;										    //	����д��GRAM��ָ��
        lcddev.setxcmd = 0X2A;										    //	����дX����ָ��
        lcddev.setycmd = 0X2B;										    //	����дY����ָ��
        lcddev.width = 480;											    //	���ÿ��480
        lcddev.height = 272;										    //	���ø߶�272
    }
    
    LCD_Scan_Dir(DFT_SCAN_DIR);											//	Ĭ��ɨ�跽��
}

/**********************************************************************************************************
�������ƣ����ô���,���Զ����û������굽�������Ͻ�(sx,sy).
���������sx,sy:������ʼ����(���Ͻ�)width,height:���ڿ�Ⱥ͸߶�,�������0!!
�����������
�������أ���
**********************************************************************************************************/
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
    u16 twidth, theight;
	
    twidth = sx + width - 1;
    theight = sy + height - 1;
	
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);
}

/**********************************************************************************************************
�������ƣ���ʼ��lcd
�����������
�����������
�������أ���
**********************************************************************************************************/
void LCD_Init(void)
{
    vu32 i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	//	ʹ��PD,PE,PF,PGʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE 
						 | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE); 
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 				//	ʹ��FSMCʱ��
    
	//	PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 
                                | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	����
    GPIO_Init(GPIOD, &GPIO_InitStructure);								//	��ʼ��

	//	PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
                                | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;						
                                
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	����
    GPIO_Init(GPIOE, &GPIO_InitStructure);								//	��ʼ��

	//	LCD_RS PG1,FSMC_A11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	����
    GPIO_Init(GPIOG, &GPIO_InitStructure);								//	��ʼ��

	//	LCD_CS PF12,FSMC_NE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	����
    GPIO_Init(GPIOG, &GPIO_InitStructure);								//	��ʼ��

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC); 			
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC); 

    GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);


    readWriteTiming.FSMC_AddressSetupTime = 0x0F;	 					//	��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;				 		//	��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
    readWriteTiming.FSMC_DataSetupTime = 60;							//	���ݱ���ʱ��Ϊ60��HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 			//	ģʽA


    writeTiming.FSMC_AddressSetupTime = 9;	     						//	��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns
    writeTiming.FSMC_AddressHoldTime = 0x00;	 						//	��ַ����ʱ�䣨A
    writeTiming.FSMC_DataSetupTime = 8;		 							//	���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 				//	ģʽA


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;			//  ��������ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6],[7]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 	// 	���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; 				// 	FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;		//	�洢�����ݿ��Ϊ16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; 	// 	FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;		//  �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 		// 	��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; 		//	��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  				//	дʱ��

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  						//	��ʼ��FSMC����

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  						// 	ʹ��BANK1

    LCD_delayms(100); 												    // 	delay 50 ms
    
    LCD_WR_REG(0x2B);		
    LCD_WR_DATA(0);		                                        
    LCD_delayus(10);
    
    //  Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WR_REG(0xE2);		
    LCD_WR_DATA(0x1D);		                                            //  ����1
    LCD_WR_DATA(0x02);		                                            //  ����2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WR_DATA(0x04);		                                            //  ����3 Validate M and N values
    LCD_delayus(100);
    
    LCD_WR_REG(0xE0);		                                            //  Start PLL command
    LCD_WR_DATA(0x01);		                                            //  enable PLL
    LCD_delayms(10);
    
    LCD_WR_REG(0xE0);		                                            //  Start PLL command again
    LCD_WR_DATA(0x03);		                                            //  now, use PLL output as system clock
    LCD_delayms(12);
    
    LCD_WR_REG(0x01);		                                            //  ��λ
    LCD_delayms(10);

    LCD_WR_REG(0xE6);		                                            //  ��������Ƶ��,33Mhz
    LCD_WR_DATA(0x00);  
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x16);

    LCD_WR_REG(0xB0);		                                            //  ����LCDģʽ
    LCD_WR_DATA(0x20);		                                            //  24λģʽ
    LCD_WR_DATA(0x00);		                                            //  TFT ģʽ

    LCD_WR_DATA((SSD_HOR_RESOLUTION) >> 8);                     	    //  ����LCDˮƽ����
    LCD_WR_DATA(SSD_HOR_RESOLUTION);
    LCD_WR_DATA((SSD_VER_RESOLUTION) >> 8);                     	    //  ����LCD��ֱ����
    LCD_WR_DATA(SSD_VER_RESOLUTION);
    LCD_WR_DATA(0x00);		                                            //  RGB����

    LCD_WR_REG(0xB4);		                                            //  Set horizontal period
    LCD_WR_DATA((SSD_HT) >> 8);
    LCD_WR_DATA(SSD_HT);
    LCD_WR_DATA(SSD_HPS >> 8);
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xB6);		                                            //  Set vertical period
    LCD_WR_DATA((SSD_VT) >> 8);
    LCD_WR_DATA(SSD_VT);
    LCD_WR_DATA(SSD_VPS >> 8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_PULSE_WIDTH);
    LCD_WR_DATA((SSD_VER_FRONT_PORCH>>8)&0x00FF);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH&0x00FF);

    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0X05);	                                                //  GPIO[1:0]=01,����LCD����

    LCD_WR_REG(0xB8);	                                                //  ����GPIO����
    LCD_WR_DATA(0x07);	                                                //  2��IO�����ó����
    LCD_WR_DATA(0x01);	                                                //  GPIOʹ��������IO����
  
    LCD_WR_REG(0xF0);	                                                //  ����SSD1963��CPU�ӿ�Ϊ16bit
    LCD_WR_DATA(0x03);                                  	            //  16-bit(565 format) data for 16bpp

    LCD_WR_REG(0x29);	                                                //  ������ʾ
    
    //  ����PWM���  ����ͨ��ռ�ձȿɵ�
    LCD_WR_REG(0xD0);	                                                //  �����Զ���ƽ��DBC
    LCD_WR_DATA(0x0d);	                                                //  disable

    LCD_WR_REG(0xBE);	                                                //  ����PWM���
    LCD_WR_DATA(0x06);	                                                //  1����PWMƵ��
    LCD_WR_DATA(0x80);	                                                //  2����PWMռ�ձ�
    LCD_WR_DATA(0x01);	                                                //  3����C
    LCD_WR_DATA(0xF0);	                                                //  4����D
    LCD_WR_DATA(0x00);	                                                //  5����E
    LCD_WR_DATA(0x00);	                                                //  6����F

    LCD_WR_REG(0xB8);	                                                //  ����GPIO����
    LCD_WR_DATA(0x03);	                                                //  2��IO�����ó����
    LCD_WR_DATA(0x01);	                                                //  GPIOʹ��������IO����


    LCD_WR_REG(0x29);	                                                //  ������ʾ
    
    LCD_Display_Dir(1);												    //	Ĭ��Ϊ����

    LCD_Clear(WHITE);
}


/**********************************************************************************************************
�������ƣ���������
���������color:Ҫ���������ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_Clear(u16 color)
{
    u32 index = 0;
	
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height; 										//	�õ��ܵ���
	
    LCD_SetCursor(0x00, 0x0000);									    //	���ù��λ��
    
    LCD_WriteRAM_Prepare();     									    //	��ʼд��GRAM
    
    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}


/**********************************************************************************************************
�������ƣ���ָ����������䵥����ɫ
���������(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
		  color:Ҫ������ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;
	
    xlen = ex - sx + 1;
    
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      									    //	���ù��λ��
        
        LCD_WriteRAM_Prepare();     								    //	��ʼд��GRAM
        
        for(j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;									    //	��ʾ��ɫ
        }
    }
}


/**********************************************************************************************************
�������ƣ���ָ�����������ָ����ɫ��
���������(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
		  color:Ҫ������ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
	
    width = ex - sx + 1; 												//	�õ����Ŀ��
    height = ey - sy + 1;												//	�߶�
	
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);   									//	���ù��λ��
        LCD_WriteRAM_Prepare();     									//	��ʼд��GRAM
		
        for(j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j]; 						//	д������
        }
    }
}


/**********************************************************************************************************
�������ƣ�����
���������x1,y1:�������
          x2,y2:�յ�����
�����������
�������أ���
**********************************************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    
    delta_x = x2 - x1; 													//	������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    
    if(delta_x > 0)
    {
        incx = 1; 														//	���õ�������
    }
    else if(delta_x == 0)
    {
        incx = 0; 														//	��ֱ��
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if(delta_y > 0)
    {
        incy = 1;
    }
    else if(delta_y == 0)
    {
        incy = 0; 														//	ˮƽ��
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if( delta_x > delta_y)
    {
        distance = delta_x; 											//	ѡȡ��������������
    }
    else 
    {
        distance = delta_y;
    }
    
    for(t = 0; t <= distance + 1; t++ ) 								//	�������
    {
        LCD_DrawPoint(uRow, uCol, color); 								//	����
        xerr += delta_x ;
        yerr += delta_y ;
        
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}


/**********************************************************************************************************
�������ƣ�������
���������(x1,y1),(x2,y2):���εĶԽ�����
�����������
�������أ���
**********************************************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}


/**********************************************************************************************************
�������ƣ���ָ��λ�û�һ��ָ����С��Բ
���������(x,y):���ĵ�
		  r    :�뾶
�����������
�������أ���
**********************************************************************************************************/
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    int di;
    
	a = 0;
	b = r;
    
	di = 3 - (r << 1);       											//	�ж��¸���λ�õı�־
    
	while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b, color);        					//	5
        LCD_DrawPoint(x0 + b, y0 - a, color);        					//	0
        LCD_DrawPoint(x0 + b, y0 + a, color);        					//	4
        LCD_DrawPoint(x0 + a, y0 + b, color);        					//	6
        LCD_DrawPoint(x0 - a, y0 + b, color);        					//	1
        LCD_DrawPoint(x0 - b, y0 + a, color);
        LCD_DrawPoint(x0 - a, y0 - b, color);        					//	2
        LCD_DrawPoint(x0 - b, y0 - a, color);        					//	7
        
		a++;
        
		//	ʹ��Bresenham�㷨��Բ
        if(di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}


/**********************************************************************************************************
�������ƣ���ʾ�����ַ�
���������x0��y0    ��ʼ����
		  pcStr     ָ��
		  PenColor  ������ɫ
		  BackColor ���屳��
�����������
�������أ���
����˵������ʾ�����ַ���������������ܵ�������	
**********************************************************************************************************/
void LCD_ShowHzString(u16 x, u16 y, u8 *c, u16 PenColor, u16 BackColor)
{
    int i, j;
    u8 buffer[32];
    u16 tmp_char=0;
    unsigned int c1, c2;
    unsigned int c3 = 0;
    unsigned char * c4=NULL;
    
    c4 = c4;
    c1 = *c;	                                                        //  ����
    c2 = *(c + 1);	                                                    //  λ��	�޸���ȷ
    c3 = ((c1 - 0xa1)*0x5e + (c2 - 0xa1))*0x20;                         //  �������ֿ��еĵ�ַ
    c4 = (unsigned char*)&Hzk16[c3];                                    //  �����ָ��
		  
	for(i=0;i<32;i++)
    {
        buffer[i] = Hzk16[c3+i];
    }

	for (i = 0; i < 16; i++)		 
	{
		tmp_char = buffer[i*2];
		tmp_char = (tmp_char << 8);
		tmp_char |= buffer[2*i + 1];                                    //  ����tmp_char�洢��һ�еĵ�������

		for (j = 0; j < 16; j++)
		{
			if((tmp_char >> 15 - j) & 0x01 == 0x01)
			{
				LCD_DrawPoint(x + j, y + i, PenColor);                  //  �ַ���ɫ
			}
			else
			{
				LCD_DrawPoint(x + j, y + i, BackColor);                 //  ������ɫ
			}
		}
	}

}


/**********************************************************************************************************
�������ƣ���ʾASCII�뺯��
���������x��y      ��ʼ���꣨x:0~234 y:0~308��
		  num       �ַ�ASCII��ֵ
		  size      �ַ���С��ʹ��Ĭ��8*16
		  PenColor  ������ɫ
		  BackColor ���屳����ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor)
{       
    u8 temp, t1, t;
	u16 y0 = y;
    
	u8 csize = (size/8 + ((size%8)? 1 : 0))*(size/2);		            //  �õ�����һ���ַ���Ӧ������ռ���ֽ���	
    
    //  �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
 	num = num - ' ';                                                    
	
    for(t = 0; t < csize; t++)
	{   
		if (size == 12)
        {
            temp = asc2_1206[num][t]; 	 	                            //  ����1206����
        }
        
		else if(size == 16) 
        {
            temp = asc2_1608[num][t];	                                //  ����1608����
        }
        
		else if(size == 24)
        {
            temp = asc2_2412[num][t];	                                //  ����2412����
        }
		else 
        {
            return;								                        //  û�е��ֿ�
        }
        
		for(t1 = 0; t1 < 8; t1++)
		{			    
			if(temp&0x80)
            {
                LCD_Fast_DrawPoint(x, y, PenColor);
            }
            
			else
            {
                LCD_Fast_DrawPoint(x, y, BackColor);
            }
            
			temp <<= 1;
            
			y++;
			
            if (y >= lcddev.height)
            {
                return;		//��������
            }
            
			if((y - y0) == size)
			{
				y = y0;
				
                x++;
				
                if(x >= lcddev.width)
                {
                    return;	//��������
                }
				break;
			}
		}  	 
	}  	    	
}


/**********************************************************************************************************
�������ƣ���ʾ1���ַ�������
���������x��y      ��ʼ����
	      p         ָ���ַ�����ʼ��ַ
		  PenColor  �ַ���ɫ
		  BackColor ������ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
{   
	uint8_t size = 16;                                                  //  �ַ���СĬ��16*8
	 
    if(x > lcddev.width)                                                //  ����X��������С��λ������
    {
        x = 0;
        y += size;
    }			         
    
    if(y > lcddev.height)                                               //  ����Y��������С��λ���ص�ԭ�㣬��������
    {
        y = x = 0;
        LCD_Clear(WHITE);
    }	 
    
    LCD_ShowChar(x, y, *p, size, PenColor, BackColor);			   		//	0��ʾ�ǵ��ӷ�ʽ
}


/**********************************************************************************************************
�������ƣ���ʾ�ַ���
���������x,y			�������
		  u8 *pcStr		�ַ�
		  PenColor  	�ַ���ɫ
		  BackColor 	������ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_ShowString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
    while(*pcStr)
	{
		if(*pcStr > 0xa1)                                               //  ��ʾ����
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;	
		}
		else                                                            //  ��ʾ�ַ�
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	
	}	
}

/**********************************************************************************************************
�������ƣ�LCD_DrawPoint
���������x.y:���������
		  color����ɫ
�����������
�������أ���
**********************************************************************************************************/
void LCD_DrawOnrPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	LCD_SetCursor(xsta, ysta);  		                                //  ���ù��λ��
	LCD_WriteRAM_Prepare();          	                                //  ��ʼд��GRAM
	LCD_WriteRAM(color); 

}


/**********************************************************************************************************
�������ƣ���ʾͼƬ
���������StartX     ����ʼ����
          StartY     ����ʼ����
          EndX       �н�������
          EndY       �н�������
          pic        ͼƬͷָ��
�����������
�������أ���
**********************************************************************************************************/
void LCD_DrawPicture(u16 StartX, u16 StartY, u16 Xend, u16 Yend, u8 *pic)
{
    static	u16 i = 0, j = 0;
    
    u16 *bitmap = (u16 *)pic;
    
    for(j = 0; j < Yend - StartY; j++)
    {
        for(i = 0; i < Xend - StartX; i++)
        {
            LCD_DrawOnrPoint(StartX + i, StartY + j, *bitmap++);
        }
    }
}
