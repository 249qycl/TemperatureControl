#ifndef _24C02_H_
#define _24C02_H_

#define Eeprom_i2c            I2C2
#define Eeprom_i2c_Rcc        RCC_APB1Periph_I2C2

#define Eerpom_i2c_port       GPIOF
#define Eerpom_i2c_Rcc        RCC_AHB1Periph_GPIOF

#define Eeprom_i2c_scl        GPIO_Pin_1
#define Eeprom_i2c_SCLsource  GPIO_PinSource1
#define Eeprom_i2c_SCL_AF     GPIO_AF_I2C2

#define Eeprom_i2c_sda        GPIO_Pin_0
#define Eeprom_i2c_SDAsource  GPIO_PinSource0
#define Eeprom_i2c_SDA_AF     GPIO_AF_I2C2




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xA0
#define I2C_PageSize           8

#define EEPROM_Block_ADDRESS 0xA0   /* Device Address */

void I2C_24c02Init(void);
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);
void I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);


/************************”√ªß≥Ã–Ú**********************************/
typedef struct
{
	uint8 alarm;
	char str[30];
	float a;
	int b;
}DATA;

typedef union
{
	DATA data;
	u8   buff[sizeof(DATA)/sizeof(char)];
}EEPROM;
extern EEPROM mydata;

extern uint8 save_flag;
void read_user_data(void);
void save_user_data(void);

#endif
