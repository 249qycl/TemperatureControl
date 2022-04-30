#include "include.h"


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	SysTick_Init();
	UART3_Configuration();
	I2C_24c02Init();
	Remote_Init();
	LEDGpio_Init0();
	PIDInit();
	TIM3_PWM_Init(1000 - 1, 84 - 1);
	LCD_Init();
	LCD_Clear(BLACK);
	ds18b20_init_show();
	delay_ms(2000);
	LCD_Clear(BLACK);
	TIM_SetCompare3(TIM3, 1000);
	Scope_init();
	Vitta_show();
	delay_ms(100);
	read_user_data();
	KEYGpio_Init();	
	while (1)
	{
		temperature = DS18B20GetTemp();
		TemperControl(temperature / 10);
		Scope_show();
		key_process();
		Usual_data_show();
		Uart3_process();		
	}
}