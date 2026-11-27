#include <stm32f10x.h>

void clocktree_init(void);
void usart2_init(u32 pclk1,u32 baud);

void delay(u32 count)
{
    u32 i;
    for(i = 0; i < count; i++);
}

void clocktree_init(void)
{
    u8 PLL=7;
    unsigned char temp = 0;
    RCC->CFGR &= 0xF8FF0000;
    RCC->CR   &= 0xFEF6FFFF; 
    RCC->CR   |= 0x00010000;
    while(!(RCC->CR>>17));
    RCC->CFGR  = 0x00000400;
    RCC->CFGR|=PLL<<18;
    RCC->CFGR |= 1<<16;
    RCC->CFGR|=1<<16;
    FLASH->ACR|= 0x32;  
    RCC->CR   |= 0x01000000;
    while(!(RCC->CR>>25));
    RCC->CFGR |= 0x00000002;
    while(temp != 0x02)
    {
        temp  = RCC->CFGR>>2;
        temp &= 0x03;    
    }
}

void usart2_init(u32 pclk1,u32 baud)
{
    float temp;
    u16 mantissa;
    u16 fraction;
    temp=(float)(pclk1*1000000)/(baud*16);
    mantissa = temp;
    fraction = (temp-mantissa)*16;
			mantissa <<= 4;
    mantissa += fraction;
    RCC->APB2ENR  |= 1<<2; 
    RCC->APB1ENR  |= 1<<17;  
    GPIOA->CRL    &= 0xFFFF00FF; 
    GPIOA->CRL    |= 0x00008B00;
    RCC->APB1RSTR |= 1<<17;  
    RCC->APB1RSTR &= ~(1<<17);
    USART2->BRR    = mantissa;   
    USART2->CR1   |= 0x2008;    
}

int main(void)
{
    clocktree_init();
    usart2_init(36, 9600);
    delay(2000000);
	  
    while(1)
    {
        USART2->DR = 0x41;
        delay(100);
        USART2->DR = 0x42;
        delay(50000);
        delay(1000000);
    }
}
