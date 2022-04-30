#include "include.h"

SCOPE Scope;


void Scope_init()
{
	static u8 re_init=0;
	u8 i;
	if(re_init==0)
	{
		Scope=(SCOPE)
		{
			.key=1,
			.step=1,
			.stop=1,
			.auto_switch=1,
			.cursor_seat=50,
			.state={1,1,0},
			.divide={60,50,40},
			.data={0},
			.color={RED,GREEN,BRED},//YELLOW  GBLUE;
			.gear={5,5,5},
			.var_name={"数据:","数据:","数据:"},
		};	
		re_init=1;
	} 
	//    lcd_clear(BACK_C);
	lcd_rectangle(0,0,SCOPE_W+2,SCOPE_H+2,WHITE);
	lcd_rectangle(1,1,SCOPE_W,SCOPE_H,BLACK);
	for(i=0;i<(SCOPE_W+2);i++)
	{
		if(i%4==0||i%4==1)
			LCD_Fast_DrawPoint(i,MID_LINE,WHITE);//0线
	}
}



void Insert_to_queue()
{ 
	u8 temp,i;
	for(i=0;i<SCOPE_VAR_NUM;i++)
	{
		if(Scope.state[i]==0)
			Scope.data[i]=0;
		temp=Scope.img[Scope.rear[i]][i];
		if(temp!=61&&temp!=0)
			LCD_Fast_DrawPoint(Scope.rear[i]+1,temp,BLACK);
		else if(temp!=0)
		{
			if((Scope.rear[i]+1)%4==0||(Scope.rear[i]+1)%4==1)
				LCD_Fast_DrawPoint(Scope.rear[i]+1,61,WHITE);
		}
		if(Scope.auto_switch==1)
			Scope.divide[i]=abs(Scope.data[i])>Scope.divide[i]?abs(Scope.data[i]):Scope.divide[i];
			
			Scope.cursor_data[Scope.rear[i]][i]=Scope.data[i];
			Scope.data[i]=Scope.data[i]>Scope.divide[i]?Scope.divide[i]:(Scope.data[i]<-Scope.divide[i]?-Scope.divide[i]:Scope.data[i]);
			temp=61 - Scope.data[i]*60/Scope.divide[i];
			Scope.img[Scope.rear[i]][i]=temp;
			LCD_Fast_DrawPoint(Scope.rear[i]+1,temp,Scope.color[i]);
			Scope.rear[i]=(Scope.rear[i]+1)%SCOPE_W;
	}
}

void Cursor_move(u8 seat)
{
	static u8 last_seat=1;
	u8 temp,i;
	
	seat=seat>SCOPE_W?SCOPE_W:(seat<1?1:seat);
	for(i=1;i<SCOPE_H;i++)
	{    
		LCD_Fast_DrawPoint(last_seat,i,BLACK);
		LCD_Fast_DrawPoint(seat,i,WHITE);
	}
	for(i=0;i<SCOPE_VAR_NUM;i++)
	{
		temp=Scope.img[last_seat][i];
		LCD_Fast_DrawPoint(last_seat,temp,Scope.color[i]);    
	}
	last_seat=seat;
}

void Scope_data_show()
{
	u8 i;
	for(i=0;i<SCOPE_VAR_NUM;i++)
	{
		LCD_Show(0  ,200+i*20,Scope.color[i],BLACK,"%s%3d",Scope.var_name[i],Scope.cursor_data[Scope.cursor_seat][i]);
	}  
}

void Scope_show()
{
	static u8 frent=0;
	if(Scope.re_window==0)
	{
		Scope_init();
		Scope.re_window=1;
	} 
	if(Scope.stop==1)
	{
		if(frent++==10)
		{
			Insert_to_queue();
			frent=0;
		}
	}
	
	Cursor_move(Scope.cursor_seat);
	Scope_data_show();
}
