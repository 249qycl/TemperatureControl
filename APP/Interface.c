#include "include.h"


//色彩渐变
s16 RGB_turn(s16 Color1,s16 Color2,u8 step,u8 N)//1---2
{
    s16 R,G,B;
    R=(Color1>>11)+((Color2>>11)-(Color1>>11))*N/step;
    G=((Color1&0x07ff)>>5)+(((Color2&0x07ff)>>5)-((Color1&0x07ff)>>5))*N/step;
    B=(Color1&0x001f)+((Color2&&0x001f)-(Color1&0x001f))*N/step;
    return (R<<11)|(G<<5)|B;
}




void Vitta_show(void)
{
	u8 i;
	s16 temp;
	lcd_rectangle(X_SEAT-70,0,180,170,WHITE);
	for(i=0;i<6;i++)
	{
		temp=RGB_turn(BLUE,WHITE,20,i*2);
		lcd_rectangle(X_SEAT,Y_SEAT+50+(6-i)*10,10,10,temp);
		temp=RGB_turn(RED,WHITE,20,i*2);
		lcd_rectangle(X_SEAT,Y_SEAT+i*10,10,10,temp);
	}
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT-4,5,RED);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT-4,4,RED);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT+120,5,BLUE);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT+120,4,BLUE);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT+120,3,BLUE);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT+120,2,BLUE);
	LCD_Draw_Circle(X_SEAT+5,Y_SEAT+120,1,BLUE);
	LCD_Show(245, 0 ,GBLUE,BLACK, "02-07");
	LCD_Show(245, 20,GBLUE,BLACK,"组员:");
	LCD_Show(245, 40,GBLUE,BLACK,"张玉亮");
	LCD_Show(245, 60,GBLUE,BLACK,"杨子强");
	LCD_Show(245, 80,GBLUE,BLACK,"袁  明");
}

void Vitta_cursor_show(float value)
{
	static u8 last_seat=0;
	static u8 turn_flag=1;
	u8 seat;
	LCD_DrawLine(X_SEAT+4,120+Y_SEAT,X_SEAT+4,last_seat+Y_SEAT,WHITE);
	LCD_DrawLine(X_SEAT+5,120+Y_SEAT,X_SEAT+5,last_seat+Y_SEAT,WHITE);
	LCD_DrawLine(X_SEAT+6,120+Y_SEAT,X_SEAT+6,last_seat+Y_SEAT,WHITE);
	seat=(u8)(110-((u16)value-25)*11*10/(40-25));
	last_seat=seat;	
	
	if(turn_flag==1)
	{
		LCD_DrawLine(X_SEAT+4,120+Y_SEAT,X_SEAT+4,seat+Y_SEAT,GREEN);
		LCD_DrawLine(X_SEAT+5,120+Y_SEAT,X_SEAT+5,seat+Y_SEAT,GREEN);
		LCD_DrawLine(X_SEAT+6,120+Y_SEAT,X_SEAT+6,seat+Y_SEAT,GREEN);
	}
	if(value>(Alarm_value+0.5)||value<(Alarm_value-0.5))
		turn_flag=!turn_flag;
	else
		turn_flag=1;
	LCD_Show(X_SEAT-60, Y_SEAT+130,BLUE,WHITE,"实时温度:%2.2f度",value);
	LCD_Show(0,180,BLUE,WHITE,"设备号:%c%c",Device_str[9],Device_str[10]);
	LCD_Show(0,140,WHITE,BLACK,"    智能仪器仪表课程设计");
	LCD_Show(0,160,WHITE,BLACK,"基于STM32的温度采集控制系统");
}