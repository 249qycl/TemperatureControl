#ifndef _KEY_H_
#define _KEY_H_

//  按键引脚定义
//KEY_S1      PB2
//KEY_S2      PA0
//KEY_S3      PC2
//KEY_S4      PC0
//KEY_S5      PE4

#define  	KEY_S1_READ     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2)
#define  	KEY_S2_READ     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define  	KEY_S3_READ     GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define  	KEY_S4_READ     GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define  	KEY_S5_READ     GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)

void KEYGpio_Init(void);
uint8 key_read();
#endif
