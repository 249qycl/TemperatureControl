#include "include.h"

USARTDATA Uart3;
unsigned char ReceiveBuffer[300] = {0};
/**********************************************************************************************************
�������ƣ�UART3����
�����������
�����������
�������أ���
**********************************************************************************************************/
// USART3_TX	 PB10	//  out
// USART3_RX	 PB11	//  in
void UART3_Configuration(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
    
    Uart3.ReceiveFinish = 0;
    Uart3.RXlenth = 0;
    Uart3.Time = 0;
    Uart3.Rxbuf = ReceiveBuffer;

	//  ����GPIO_D��ʱ�� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
	//  ��������3��ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);


	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART3, &USART_InitStructure);

	/* ʹ�ܴ���3 */
	USART_Cmd(USART3, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    /* NVIC configuration */
    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART */
    USART_Cmd(USART3, ENABLE);
}

/**********************************************************************************************************
�������ƣ�putchar�����ض���
�����������
�����������
�������أ���
**********************************************************************************************************/
int fputc(int ch, FILE *f)
{
	USART3->SR; // ��ֹ��λ���޷���ӡ���ַ�

	USART_SendData(USART3, (u8)ch);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		;

	return (ch);
}

/**********************************************************************************************************
�������ƣ�USART3�������ݺ���
������������������׵�ַ�����ݳ���
�����������
**********************************************************************************************************/
void USART3_Senddata(unsigned char *Data, unsigned int length)
{
	while (length--)
	{
		USART_SendData(USART3, *Data++);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			;
	}
}
/*******************************�û�����*****************************************/

char Device_str[30]="������ʾ+01";
char format_s[30]="{$��ǰ�¶�Ϊ:";
char format_e[30]="��$}";
void Uart3_process(void)
{
//	char tempstr[30]="������ʾ+";//����ռ�����ֽ�

	char format_cmd[30]={0};
	char temp_str[30]={0};
	
	uint8 seat=0,i;
	if (Uart3.ReceiveFinish)
	{
		USART3_Senddata(Uart3.Rxbuf, Uart3.RXlenth);
		printf("\r\n");
		
		memset(format_cmd,'\0',30);
		memcpy(format_cmd,Uart3.Rxbuf,13);
		memcpy(temp_str,Uart3.Rxbuf,9);
		
		
		if (0 == strcmp((char *)Uart3.Rxbuf,Device_str))		
			printf("%s%2.2f%s\r\n",format_s,(float)temperature/100,format_e);
		else if(0==strcmp("��ַ�޸�+",temp_str))
		{
			if(Uart3.Rxbuf[9]==Device_str[9]&&Uart3.Rxbuf[10]==Device_str[10])
			{
				Device_str[9] =Uart3.Rxbuf[12];
				Device_str[10]=Uart3.Rxbuf[13];
				
				if(Device_str[9]>'9'||Device_str[9]<'0'||Device_str[10]>'9'||Device_str[10]<'0')
					printf("�豸�Ų����ϱ�׼!\r\n");
				else
				{	
					printf("%s\r\n",Device_str);
					printf("�豸��ַ�޸ĳɹ�!\r\n");
				}
			}
			else
				printf("�豸��ַ��ƥ��!\r\n");
		}
		else if(0 == strcmp("��ʾ��ʽ����:",format_cmd))
		{	
			while(Uart3.Rxbuf[seat]!='+')
			{
				seat++;
			}
			
			memset(format_s,'\0',30);
			memset(format_e,'\0',30);
			
			for(i=13;i<seat;i++)
			{
				format_s[i-13]=Uart3.Rxbuf[i];
			}
			for(i=seat+1;i<Uart3.RXlenth;i++)
			{
				format_e[i-seat-1]=Uart3.Rxbuf[i];
			}
			
			printf("��ʾ��ʽ�޸ĳɹ�!\r\n");
		}
		else
			printf("ָ�����!\r\n");
		memset(Uart3.Rxbuf,'\0',Uart3.RXlenth);
		Uart3.ReceiveFinish = 0;
		Uart3.RXlenth = 0;
	}
}
