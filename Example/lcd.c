#include "LCD.h"
#include "Hzk16song.h"//汉字库
#include "font.h"

//字符缓冲区
char strbuff[50];

//	LCD结构体，默认为横屏
lcd_dev lcddev;

/**********************************************************************************************************
函数名称：液晶us延时函数
输入参数：时间
输出参数：无
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
函数名称：LCD延时函数（大约1ms）
输入参数：时间
输出参数：无
函数返回：无
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
函数名称：写寄存器函数
输入参数：寄存器值
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG = regval; //写入要写的寄存器序号
}

/**********************************************************************************************************
函数名称：写数据函数
输入参数：数据
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM = data;
}


/**********************************************************************************************************
函数名称：读数据函数
输入参数：无
输出参数：无
函数返回：读到的值
**********************************************************************************************************/
u16 LCD_RD_DATA(void)
{
    vu16 ram;															//	防止被优化
	
    ram = LCD->LCD_RAM;
    
	return ram;
}


/**********************************************************************************************************
函数名称：写寄存器数据函数
输入参数：寄存器值、数据
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;												//	写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;										//	写入数据
}

/**********************************************************************************************************
函数名称：读寄存器数据函数
输入参数：寄存器值
输出参数：无
函数返回：读到的数据
**********************************************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);												//	写入要读的寄存器序号
    LCD_delayus(5);
	
    return LCD_RD_DATA();												//	返回读到的值
}

/**********************************************************************************************************
函数名称：写GRAM命令函数
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}


/**********************************************************************************************************
函数名称：写RAM数据函数
输入参数：颜色值
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;											//	写十六位GRAM
}


/**********************************************************************************************************
函数名称：RGB转换函数
输入参数：GBR格式的颜色值
输出参数：无
函数返回：RGB格式的颜色值
函数说明：从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
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
函数名称：延时函数
输入参数：时间
输出参数：无
函数返回：无
**********************************************************************************************************/
void opt_delay(u8 i)
{
    while(i--);
}


/**********************************************************************************************************
函数名称：读取个某点的颜色值
输入参数：x,y:坐标
输出参数：无
函数返回：此点的颜色
**********************************************************************************************************/
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 r = 0;
	
    if(x >= lcddev.width || y >= lcddev.height)
    {
        return 0;														//	超过了范围,直接返回
    }
    
    LCD_SetCursor(x, y);
    
    LCD_WR_REG(0X2E); 												    //	发送读GRAM指令
    
    r = LCD_RD_DATA();													//	dummy Read
    
    return r;
}


/**********************************************************************************************************
函数名称：设置光标位置
输入参数：Xpos:横坐标
		  Ypos:纵坐标
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if(lcddev.dir == 0) 											    //	x坐标需要变换
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
函数名称：设置LCD的自动扫描方向
输入参数：方向
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
	
	//	横屏时，1963不改变扫描方向！竖屏时1963改变方向
    if(lcddev.dir == 0) 
    {
        switch(dir)														//	方向转换
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
	
	//	1963,特殊处理
    switch(dir)
    {
        case 	L2R_U2D:											//	从左到右,从上到下
                regval |= (0 << 7) | (0 << 6) | (0 << 5);
                break;
                
        case 	L2R_D2U:											//	从左到右,从下到上
                regval |= (1 << 7) | (0 << 6) | (0 << 5);
                break;
                
        case 	R2L_U2D:											//	从右到左,从上到下
                regval |= (0 << 7) | (1 << 6) | (0 << 5);
                break;
                
        case 	R2L_D2U:											//	从右到左,从下到上
                regval |= (1 << 7) | (1 << 6) | (0 << 5);
                break;
                
        case 	U2D_L2R:											//	从上到下,从左到右
                regval |= (0 << 7) | (0 << 6) | (1 << 5);
                break;
                
        case 	U2D_R2L:											//	从上到下,从右到左
                regval |= (0 << 7) | (1 << 6) | (1 << 5);
                break;
                
        case 	D2U_L2R:											//	从下到上,从左到右
                regval |= (1 << 7) | (0 << 6) | (1 << 5);
                break;
                
        case 	D2U_R2L:											//	从下到上,从右到左
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
函数名称：画点
输入参数：x,y	坐标
		  color 颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_SetCursor(x, y);												//	设置光标位置
    LCD_WriteRAM_Prepare();												//	开始写入GRAM
    LCD->LCD_RAM = color;
}

/**********************************************************************************************************
函数名称：快速画点
输入参数：x,y:	坐标
		  color 颜色
输出参数：无
函数返回：无
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

//设置LCD显示方向
//dir:0,竖屏；1,横屏
/**********************************************************************************************************
函数名称：设置LCD显示方向
输入参数：dir:0,竖屏；1,横屏
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Display_Dir(u8 dir)
{
    if(dir == 0)														//	竖屏
    {
        lcddev.dir = 0;													//	竖屏
        lcddev.wramcmd = 0X2C;										    //	设置写入GRAM的指令
        lcddev.setxcmd = 0X2B;										    //	设置写X坐标指令
        lcddev.setycmd = 0X2A;										    //	设置写Y坐标指令
        lcddev.width = 272;											    //	设置宽度272
        lcddev.height = 480;										    //	设置高度480
    }
    else 																//	横屏
    {
        lcddev.dir = 1;													//	横屏
        lcddev.wramcmd = 0X2C;										    //	设置写入GRAM的指令
        lcddev.setxcmd = 0X2A;										    //	设置写X坐标指令
        lcddev.setycmd = 0X2B;										    //	设置写Y坐标指令
        lcddev.width = 480;											    //	设置宽度480
        lcddev.height = 272;										    //	设置高度272
    }
    
    LCD_Scan_Dir(DFT_SCAN_DIR);											//	默认扫描方向
}

/**********************************************************************************************************
函数名称：设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
输入参数：sx,sy:窗口起始坐标(左上角)width,height:窗口宽度和高度,必须大于0!!
输出参数：无
函数返回：无
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
函数名称：初始化lcd
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Init(void)
{
    vu32 i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	//	使能PD,PE,PF,PG时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE 
						 | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE); 
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 				//	使能FSMC时钟
    
	//	PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 
                                | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);								//	初始化

	//	PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
                                | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;						
                                
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);								//	初始化

	//	LCD_RS PG1,FSMC_A11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);								//	初始化

	//	LCD_CS PF12,FSMC_NE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//	复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//	推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//	100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//	上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);								//	初始化

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


    readWriteTiming.FSMC_AddressSetupTime = 0x0F;	 					//	地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;				 		//	地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_DataSetupTime = 60;							//	数据保存时间为60个HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 			//	模式A


    writeTiming.FSMC_AddressSetupTime = 9;	     						//	地址建立时间（ADDSET）为9个HCLK =54ns
    writeTiming.FSMC_AddressHoldTime = 0x00;	 						//	地址保持时间（A
    writeTiming.FSMC_DataSetupTime = 8;		 							//	数据保存时间为6ns*9个HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 				//	模式A


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;			//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 	// 	不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; 				// 	FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;		//	存储器数据宽度为16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; 	// 	FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;		//  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 		// 	读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; 		//	读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  				//	写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  						//	初始化FSMC配置

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  						// 	使能BANK1

    LCD_delayms(100); 												    // 	delay 50 ms
    
    LCD_WR_REG(0x2B);		
    LCD_WR_DATA(0);		                                        
    LCD_delayus(10);
    
    //  Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WR_REG(0xE2);		
    LCD_WR_DATA(0x1D);		                                            //  参数1
    LCD_WR_DATA(0x02);		                                            //  参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WR_DATA(0x04);		                                            //  参数3 Validate M and N values
    LCD_delayus(100);
    
    LCD_WR_REG(0xE0);		                                            //  Start PLL command
    LCD_WR_DATA(0x01);		                                            //  enable PLL
    LCD_delayms(10);
    
    LCD_WR_REG(0xE0);		                                            //  Start PLL command again
    LCD_WR_DATA(0x03);		                                            //  now, use PLL output as system clock
    LCD_delayms(12);
    
    LCD_WR_REG(0x01);		                                            //  软复位
    LCD_delayms(10);

    LCD_WR_REG(0xE6);		                                            //  设置像素频率,33Mhz
    LCD_WR_DATA(0x00);  
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x16);

    LCD_WR_REG(0xB0);		                                            //  设置LCD模式
    LCD_WR_DATA(0x20);		                                            //  24位模式
    LCD_WR_DATA(0x00);		                                            //  TFT 模式

    LCD_WR_DATA((SSD_HOR_RESOLUTION) >> 8);                     	    //  设置LCD水平像素
    LCD_WR_DATA(SSD_HOR_RESOLUTION);
    LCD_WR_DATA((SSD_VER_RESOLUTION) >> 8);                     	    //  设置LCD垂直像素
    LCD_WR_DATA(SSD_VER_RESOLUTION);
    LCD_WR_DATA(0x00);		                                            //  RGB序列

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
    LCD_WR_DATA(0X05);	                                                //  GPIO[1:0]=01,控制LCD方向

    LCD_WR_REG(0xB8);	                                                //  设置GPIO配置
    LCD_WR_DATA(0x07);	                                                //  2个IO口设置成输出
    LCD_WR_DATA(0x01);	                                                //  GPIO使用正常的IO功能
  
    LCD_WR_REG(0xF0);	                                                //  设置SSD1963与CPU接口为16bit
    LCD_WR_DATA(0x03);                                  	            //  16-bit(565 format) data for 16bpp

    LCD_WR_REG(0x29);	                                                //  开启显示
    
    //  设置PWM输出  背光通过占空比可调
    LCD_WR_REG(0xD0);	                                                //  设置自动白平衡DBC
    LCD_WR_DATA(0x0d);	                                                //  disable

    LCD_WR_REG(0xBE);	                                                //  配置PWM输出
    LCD_WR_DATA(0x06);	                                                //  1设置PWM频率
    LCD_WR_DATA(0x80);	                                                //  2设置PWM占空比
    LCD_WR_DATA(0x01);	                                                //  3设置C
    LCD_WR_DATA(0xF0);	                                                //  4设置D
    LCD_WR_DATA(0x00);	                                                //  5设置E
    LCD_WR_DATA(0x00);	                                                //  6设置F

    LCD_WR_REG(0xB8);	                                                //  设置GPIO配置
    LCD_WR_DATA(0x03);	                                                //  2个IO口设置成输出
    LCD_WR_DATA(0x01);	                                                //  GPIO使用正常的IO功能


    LCD_WR_REG(0x29);	                                                //  开启显示
    
    LCD_Display_Dir(1);												    //	默认为横屏

    LCD_Clear(WHITE);
}


/**********************************************************************************************************
函数名称：清屏函数
输入参数：color:要清屏的填充色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Clear(u16 color)
{
    u32 index = 0;
	
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height; 										//	得到总点数
	
    LCD_SetCursor(0x00, 0x0000);									    //	设置光标位置
    
    LCD_WriteRAM_Prepare();     									    //	开始写入GRAM
    
    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}


/**********************************************************************************************************
函数名称：在指定区域内填充单个颜色
输入参数：(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
		  color:要填充的颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;
	
    xlen = ex - sx + 1;
    
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      									    //	设置光标位置
        
        LCD_WriteRAM_Prepare();     								    //	开始写入GRAM
        
        for(j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;									    //	显示颜色
        }
    }
}


/**********************************************************************************************************
函数名称：在指定区域内填充指定颜色块
输入参数：(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
		  color:要填充的颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
	
    width = ex - sx + 1; 												//	得到填充的宽度
    height = ey - sy + 1;												//	高度
	
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);   									//	设置光标位置
        LCD_WriteRAM_Prepare();     									//	开始写入GRAM
		
        for(j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j]; 						//	写入数据
        }
    }
}


/**********************************************************************************************************
函数名称：画线
输入参数：x1,y1:起点坐标
          x2,y2:终点坐标
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    
    delta_x = x2 - x1; 													//	计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    
    if(delta_x > 0)
    {
        incx = 1; 														//	设置单步方向
    }
    else if(delta_x == 0)
    {
        incx = 0; 														//	垂直线
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
        incy = 0; 														//	水平线
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if( delta_x > delta_y)
    {
        distance = delta_x; 											//	选取基本增量坐标轴
    }
    else 
    {
        distance = delta_y;
    }
    
    for(t = 0; t <= distance + 1; t++ ) 								//	画线输出
    {
        LCD_DrawPoint(uRow, uCol, color); 								//	画点
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
函数名称：画矩形
输入参数：(x1,y1),(x2,y2):矩形的对角坐标
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}


/**********************************************************************************************************
函数名称：在指定位置画一个指定大小的圆
输入参数：(x,y):中心点
		  r    :半径
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    int di;
    
	a = 0;
	b = r;
    
	di = 3 - (r << 1);       											//	判断下个点位置的标志
    
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
        
		//	使用Bresenham算法画圆
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
函数名称：显示中文字符
输入参数：x0，y0    起始坐标
		  pcStr     指向
		  PenColor  字体颜色
		  BackColor 字体背景
输出参数：无
函数返回：无
函数说明：显示汉字字符串，这个函数不能单独调用	
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
    c1 = *c;	                                                        //  区号
    c2 = *(c + 1);	                                                    //  位号	修改正确
    c3 = ((c1 - 0xa1)*0x5e + (c2 - 0xa1))*0x20;                         //  汉字在字库中的地址
    c4 = (unsigned char*)&Hzk16[c3];                                    //  换算成指针
		  
	for(i=0;i<32;i++)
    {
        buffer[i] = Hzk16[c3+i];
    }

	for (i = 0; i < 16; i++)		 
	{
		tmp_char = buffer[i*2];
		tmp_char = (tmp_char << 8);
		tmp_char |= buffer[2*i + 1];                                    //  现在tmp_char存储着一行的点阵数据

		for (j = 0; j < 16; j++)
		{
			if((tmp_char >> 15 - j) & 0x01 == 0x01)
			{
				LCD_DrawPoint(x + j, y + i, PenColor);                  //  字符颜色
			}
			else
			{
				LCD_DrawPoint(x + j, y + i, BackColor);                 //  背景颜色
			}
		}
	}

}


/**********************************************************************************************************
函数名称：显示ASCII码函数
输入参数：x，y      起始坐标（x:0~234 y:0~308）
		  num       字符ASCII码值
		  size      字符大小，使用默认8*16
		  PenColor  字体颜色
		  BackColor 字体背景颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor)
{       
    u8 temp, t1, t;
	u16 y0 = y;
    
	u8 csize = (size/8 + ((size%8)? 1 : 0))*(size/2);		            //  得到字体一个字符对应点阵集所占的字节数	
    
    //  得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
 	num = num - ' ';                                                    
	
    for(t = 0; t < csize; t++)
	{   
		if (size == 12)
        {
            temp = asc2_1206[num][t]; 	 	                            //  调用1206字体
        }
        
		else if(size == 16) 
        {
            temp = asc2_1608[num][t];	                                //  调用1608字体
        }
        
		else if(size == 24)
        {
            temp = asc2_2412[num][t];	                                //  调用2412字体
        }
		else 
        {
            return;								                        //  没有的字库
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
                return;		//超区域了
            }
            
			if((y - y0) == size)
			{
				y = y0;
				
                x++;
				
                if(x >= lcddev.width)
                {
                    return;	//超区域了
                }
				break;
			}
		}  	 
	}  	    	
}


/**********************************************************************************************************
函数名称：显示1个字符串函数
输入参数：x，y      起始坐标
	      p         指向字符串起始地址
		  PenColor  字符颜色
		  BackColor 背景颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
{   
	uint8_t size = 16;                                                  //  字符大小默认16*8
	 
    if(x > lcddev.width)                                                //  超出X轴字体最小单位，换行
    {
        x = 0;
        y += size;
    }			         
    
    if(y > lcddev.height)                                               //  超出Y轴字体最小单位，回到原点，并且清屏
    {
        y = x = 0;
        LCD_Clear(WHITE);
    }	 
    
    LCD_ShowChar(x, y, *p, size, PenColor, BackColor);			   		//	0表示非叠加方式
}


/**********************************************************************************************************
函数名称：显示字符串
输入参数：x,y			起点坐标
		  u8 *pcStr		字符
		  PenColor  	字符颜色
		  BackColor 	背景颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_ShowString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
    while(*pcStr)
	{
		if(*pcStr > 0xa1)                                               //  显示汉字
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;	
		}
		else                                                            //  显示字符
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	
	}	
}

/**********************************************************************************************************
函数名称：LCD_DrawPoint
输入参数：x.y:画点的坐标
		  color：颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawOnrPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	LCD_SetCursor(xsta, ysta);  		                                //  设置光标位置
	LCD_WriteRAM_Prepare();          	                                //  开始写入GRAM
	LCD_WriteRAM(color); 

}


/**********************************************************************************************************
函数名称：显示图片
输入参数：StartX     行起始座标
          StartY     列起始座标
          EndX       行结束座标
          EndY       列结束座标
          pic        图片头指针
输出参数：无
函数返回：无
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
