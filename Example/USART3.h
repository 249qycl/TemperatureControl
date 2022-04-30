#ifndef USART3_H
#define USART3_H


//  用于USART接收数据包
typedef struct UsartData                                                        
{		
    unsigned char *Rxbuf;
    unsigned int   RXlenth;
    unsigned char  Time;
    unsigned char  ReceiveFinish;
}USARTDATA,*PUSARTDATA;

extern USARTDATA   Uart3;

extern char Device_str[30];
// 函数声明
void UART3_Configuration(void);
void USART3_Senddata(unsigned char *Data, unsigned int length);
void Uart3_process(void);
#endif
