#include "include.h"

uint16_t EEPROM_ADDRESS;

//24C02官脚初始化
static void Gpio_24C02Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 
    
    /*!< sEE_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(Eeprom_i2c_Rcc, ENABLE);
    
    /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
    RCC_AHB1PeriphClockCmd(Eerpom_i2c_Rcc, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Reset sEE_I2C IP */
    RCC_APB1PeriphResetCmd(Eeprom_i2c_Rcc, ENABLE);
    
    /* Release reset signal of sEE_I2C IP */
    RCC_APB1PeriphResetCmd(Eeprom_i2c_Rcc, DISABLE);
    
    /*!< GPIO configuration */
    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig(Eerpom_i2c_port, Eeprom_i2c_SCLsource, Eeprom_i2c_SCL_AF);
    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig(Eerpom_i2c_port, Eeprom_i2c_SDAsource, Eeprom_i2c_SDA_AF);  
    
    /*!< Configure sEE_I2C pins: SCL */   
    GPIO_InitStructure.GPIO_Pin = Eeprom_i2c_scl;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(Eerpom_i2c_port, &GPIO_InitStructure);
    
    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = Eeprom_i2c_sda;
    GPIO_Init(Eerpom_i2c_port, &GPIO_InitStructure);
}

//I2C配置
static void I2C_Config(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    
    /*!< I2C configuration */
    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
    
    /* I2C Peripheral Enable */
    I2C_Cmd(I2C2, ENABLE);
    /* Apply I2C configuration after enabling it */
    I2C_Init(I2C2, &I2C_InitStructure);
}

//24C02初始化
void I2C_24c02Init(void)
{
    Gpio_24C02Init();

    I2C_Config();

    EEPROM_ADDRESS = EEPROM_Block_ADDRESS;
}

/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param pBuffer : pointer to the buffer  containing the data to be 
  *   written to the EEPROM.
  * @param WriteAddr : EEPROM's internal address to write to.
  * @param NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval : None
  */
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}


/**
  * @brief  Writes one byte to the I2C EEPROM.
  * @param pBuffer : pointer to the buffer  containing the data to be 
  *   written to the EEPROM.
  * @param WriteAddr : EEPROM's internal address to write to.
  * @retval : None
  */
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(Eeprom_i2c, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_MODE_SELECT));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(Eeprom_i2c, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(Eeprom_i2c, WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(Eeprom_i2c, *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(Eeprom_i2c, ENABLE);
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE
  *   cycle. The number of byte can't exceed the EEPROM page size.
  * @param pBuffer : pointer to the buffer containing the data to be 
  *   written to the EEPROM.
  * @param WriteAddr : EEPROM's internal address to write to.
  * @param NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval : None
  */
void I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(Eeprom_i2c, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(Eeprom_i2c, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(Eeprom_i2c, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(Eeprom_i2c, WriteAddr);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(Eeprom_i2c, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(Eeprom_i2c, ENABLE);
}
/**
  * @brief  Reads a block of data from the EEPROM.
  * @param pBuffer : pointer to the buffer that receives the data read 
  *   from the EEPROM.
  * @param ReadAddr : EEPROM's internal address to read from.
  * @param NumByteToRead : number of bytes to read from the EEPROM.
  * @retval : None
  */
void I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{  
    /* While the bus is busy */
  while(I2C_GetFlagStatus(Eeprom_i2c, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(Eeprom_i2c, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_MODE_SELECT));
   
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(Eeprom_i2c, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(Eeprom_i2c, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(Eeprom_i2c, ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(Eeprom_i2c, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(Eeprom_i2c, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(Eeprom_i2c, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(Eeprom_i2c, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(Eeprom_i2c, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(Eeprom_i2c);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(Eeprom_i2c, ENABLE);
}

/**
  * @brief  Wait for EEPROM Standby state
  * @param  None
  * @retval : None
  */
void I2C_EE_WaitEepromStandbyState(void)      
{
	__IO uint16_t SR1_Tmp = 0;
	
	do
	{
		/* Send START condition */
		I2C_GenerateSTART(Eeprom_i2c, ENABLE);
		/* Read I2C1 SR1 register */
		SR1_Tmp = I2C_ReadRegister(Eeprom_i2c, I2C_Register_SR1);
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(Eeprom_i2c, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	}while(!(I2C_ReadRegister(Eeprom_i2c, I2C_Register_SR1) & 0x0002));
	
	/* Clear AF flag */
	I2C_ClearFlag(Eeprom_i2c, I2C_FLAG_AF);
	
	/* STOP condition */    
	I2C_GenerateSTOP(Eeprom_i2c, ENABLE);  
}



/********************用户程序*************************/
EEPROM mydata;
uint8 save_flag=0;
void save_user_data(void)
{
	mydata.data.a=spid.SetPoint;
	mydata.data.alarm=(uint8)Alarm_value;	
	mydata.data.b=0x92;
	memcpy(mydata.data.str,Device_str,30);
	I2C_EE_BufferWrite(mydata.buff, 0, sizeof(EEPROM)/sizeof(char));
}


void read_user_data(void)
{
	I2C_EE_BufferRead(mydata.buff, 0, sizeof(EEPROM)/sizeof(char));
	if(mydata.data.b==0x92)
	{
		Alarm_value=mydata.data.alarm;
		memcpy(Device_str,mydata.data.str,30);
		spid.SetPoint=(int)mydata.data.a;
	}		
}


