#ifndef USART3_H
#define USART3_H


//  ����USART�������ݰ�
typedef struct UsartData                                                        
{		
    unsigned char *Rxbuf;
    unsigned int   RXlenth;
    unsigned char  Time;
    unsigned char  ReceiveFinish;
}USARTDATA,*PUSARTDATA;

extern USARTDATA   Uart3;

extern char Device_str[30];
// ��������
void UART3_Configuration(void);
void USART3_Senddata(unsigned char *Data, unsigned int length);
void Uart3_process(void);
#endif
