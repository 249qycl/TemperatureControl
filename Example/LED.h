#ifndef __LED_H__
#define __LED_H__

/*硬件资源引脚定义
//LED1      PB6
//LED2      PA4
//LED3      PA6
//LED4      PC6
*/
//LED1      PB6
#define  	LED1_ON      	GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define  	LED1_OFF      	GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define  	LED1_REVERSE  	GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)!GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6))

//LED2      PA4
#define  	LED2_ON      	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define  	LED2_OFF      	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define  	LED2_REVERSE  	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4))

//LED3      PA6
#define  	LED3_ON      	GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define  	LED3_OFF      	GPIO_SetBits(GPIOA, GPIO_Pin_6)
#define  	LED3_REVERSE  	GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6))

//LED4      PC6
#define  	LED4_ON      	GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define  	LED4_OFF      	GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define  	LED4_REVERSE  	GPIO_WriteBit(GPIOC, GPIO_Pin_6, (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_6))

void LEDGpio_Init0(void);
void LEDGpio_Init(void);

#endif
