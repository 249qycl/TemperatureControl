#ifndef DELAY_H
#define DELAY_H

void SysTick_Init(void);
void delay_ms(unsigned int time);
void delay_us(unsigned int time);
void TimingDelay_Decrement(void);
#endif
