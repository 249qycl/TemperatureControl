#ifndef __LCD_H
#define __LCD_H

extern char strbuff[50];

#define LCD_Show(x0,y0,Fc,Bc,...)	do{sprintf(strbuff,__VA_ARGS__);LCD_ShowString(x0,y0,(u8*)strbuff,Fc,Bc);}while(0)



#include "stm32f4xx.h"
#include <stdlib.h>

#define LCD_W 480
#define LCD_H 272

//	ɨ�跽����
#define L2R_U2D  0 														//	������,���ϵ���
#define L2R_D2U  1 														//	������,���µ���
#define R2L_U2D  2 														//	���ҵ���,���ϵ���
#define R2L_D2U  3 														//	���ҵ���,���µ���

#define U2D_L2R  4 														//	���ϵ���,������
#define U2D_R2L  5 														//	���ϵ���,���ҵ���
#define D2U_L2R  6 														//	���µ���,������
#define D2U_R2L  7 														//	���µ���,���ҵ���	 

#define DFT_SCAN_DIR  L2R_U2D  											//	Ĭ�ϵ�ɨ�跽��

//	������ɫ
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
#define BROWN 			 0XBC40 										//	��ɫ
#define BRRED 			 0XFC07 										//	�غ�ɫ
#define GRAY  			 0X8430 										//	��ɫ
//	GUI��ɫ

#define DARKBLUE      	 0X01CF											//	����ɫ
#define LIGHTBLUE      	 0X7D7C											//	ǳ��ɫ  
#define GRAYBLUE       	 0X5458								 			//	����ɫ
//	������ɫΪPANEL����ɫ

#define LIGHTGREEN     	 0X841F 										//	ǳ��ɫ
#define LGRAY 			 0XC618 										//	ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 										//	ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 										//	ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

//	LCD�ֱ�������
#define SSD_HOR_RESOLUTION		(480 - 1) 								//	LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION		(272 - 1)								//	LCD��ֱ�ֱ���

//	LCD������������
#define SSD_HOR_PULSE_WIDTH		10										//	ˮƽ����
#define SSD_HOR_BACK_PORCH		8										//	ˮƽǰ��
#define SSD_HOR_FRONT_PORCH		43										//	ˮƽ����

#define SSD_VER_PULSE_WIDTH		10										//	��ֱ����
#define SSD_VER_BACK_PORCH		12										//	��ֱǰ��
#define SSD_VER_FRONT_PORCH		4										//	��ֱǰ��

//	���¼����������Զ�����
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_FRONT_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//LCD��ַ�ṹ��
typedef struct
{
    unsigned short LCD_REG;
    unsigned short LCD_RAM;
} LCD_TypeDef;
//	ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A11��Ϊ��������������
//	ע������ʱSTM32�ڲ�������һλ����!    1111 1111 1110 = 0xFFE
#define LCD_BASE        ((u32)(0x6C000000 | 0x00000FFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


//	LCD�����ṹ��
typedef struct lcd_dev
{
    u16 width;															//	LCD ���
    u16 height;															//	LCD �߶�
    u8  dir;															//	���������������ƣ�0��������1��������
    u16	wramcmd;														//	��ʼдgramָ��
    u16 setxcmd;														//	����x����ָ��
    u16 setycmd;														//	����y����ָ��
} lcd_dev;


//	LCD����
extern lcd_dev lcddev;													//	����LCD��Ҫ����
	    

//	��������
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
