#ifndef __LCD_H
#define __LCD_H

extern char strbuff[50];

#define LCD_Show(x0,y0,Fc,Bc,...)	do{sprintf(strbuff,__VA_ARGS__);LCD_ShowString(x0,y0,(u8*)strbuff,Fc,Bc);}while(0)



#include "stm32f4xx.h"
#include <stdlib.h>

#define LCD_W 480
#define LCD_H 272

//	扫描方向定义
#define L2R_U2D  0 														//	从左到右,从上到下
#define L2R_D2U  1 														//	从左到右,从下到上
#define R2L_U2D  2 														//	从右到左,从上到下
#define R2L_D2U  3 														//	从右到左,从下到上

#define U2D_L2R  4 														//	从上到下,从左到右
#define U2D_R2L  5 														//	从上到下,从右到左
#define D2U_L2R  6 														//	从下到上,从左到右
#define D2U_R2L  7 														//	从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  											//	默认的扫描方向

//	画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 										//	棕色
#define BRRED 			 0XFC07 										//	棕红色
#define GRAY  			 0X8430 										//	灰色
//	GUI颜色

#define DARKBLUE      	 0X01CF											//	深蓝色
#define LIGHTBLUE      	 0X7D7C											//	浅蓝色  
#define GRAYBLUE       	 0X5458								 			//	灰蓝色
//	以上三色为PANEL的颜色

#define LIGHTGREEN     	 0X841F 										//	浅绿色
#define LGRAY 			 0XC618 										//	浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 										//	浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 										//	浅棕蓝色(选择条目的反色)

//	LCD分辨率设置
#define SSD_HOR_RESOLUTION		(480 - 1) 								//	LCD水平分辨率
#define SSD_VER_RESOLUTION		(272 - 1)								//	LCD垂直分辨率

//	LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH		10										//	水平脉宽
#define SSD_HOR_BACK_PORCH		8										//	水平前廊
#define SSD_HOR_FRONT_PORCH		43										//	水平后廊

#define SSD_VER_PULSE_WIDTH		10										//	垂直脉宽
#define SSD_VER_BACK_PORCH		12										//	垂直前廊
#define SSD_VER_FRONT_PORCH		4										//	垂直前廊

//	如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_FRONT_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//LCD地址结构体
typedef struct
{
    unsigned short LCD_REG;
    unsigned short LCD_RAM;
} LCD_TypeDef;
//	使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A11作为数据命令区分线
//	注意设置时STM32内部会右移一位对其!    1111 1111 1110 = 0xFFE
#define LCD_BASE        ((u32)(0x6C000000 | 0x00000FFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


//	LCD参数结构体
typedef struct lcd_dev
{
    u16 width;															//	LCD 宽度
    u16 height;															//	LCD 高度
    u8  dir;															//	横屏还是竖屏控制：0，竖屏；1，横屏。
    u16	wramcmd;														//	开始写gram指令
    u16 setxcmd;														//	设置x坐标指令
    u16 setycmd;														//	设置y坐标指令
} lcd_dev;


//	LCD参数
extern lcd_dev lcddev;													//	管理LCD重要参数
	    

//	函数声明
void LCD_delayus(unsigned int time);
void LCD_delayms(unsigned int cnt);
void LCD_WR_REG(u16 regval);
void LCD_WR_DATA(u16 data);
u16 LCD_RD_DATA(void);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_BGR2RGB(u16 c);
void opt_delay(u8 i);
u16 LCD_ReadPoint(u16 x, u16 y);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_Scan_Dir(u8 dir);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color);
void LCD_Display_Dir(u8 dir);
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height);
void LCD_Init(void);
void LCD_Clear(u16 color);
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r, u16 color);
void LCD_ShowHzString(u16 x, u16 y, u8 *c, u16 PenColor, u16 BackColor);
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor);
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor);
void LCD_ShowString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor);
void LCD_DrawOnrPoint(uint16_t xsta, uint16_t ysta, uint16_t color);
void LCD_DrawPicture(u16 StartX, u16 StartY, u16 Xend, u16 Yend, u8 *pic);

#endif
