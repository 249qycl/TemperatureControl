#ifndef DS18B20_H
#define DS18B20_H


//DS18B20引脚定义
//DS18B20         PF13		
#define  	DS18B20IO_L      	GPIO_ResetBits(GPIOF, GPIO_Pin_13)
#define  	DS18B20IO_H      	GPIO_SetBits(GPIOF, GPIO_Pin_13)		

#define  	DS18B20READ         GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)

//  函数声明   	
void DS18B20IOOUT(void);
void DS18B20IOIN(void);
unsigned char DS18B20Init(void);
void DS18B20Reset(void);
unsigned char DS18B20Check(void);
unsigned char DS18B20ReadBit(void);
unsigned char DS18B20ReadByte(void);
void DS18B20WriteByte(unsigned char dat);
void DS18B20Start(void);
int  DS18B20GetTemp(void);


/************************用户程序**********************************/
extern float Alarm_value;
extern int temperature;
void ds18b20_init_show(void);
void Alarm_temp(float temp);
void Usual_data_show();
#endif















