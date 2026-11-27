#include <stm32f10x.h>

void clocktree_init(void);
void usart2_init(u32 pclk1, u32 baud);
void usart1_init(u32 pclk2, u32 baud);
void usart_print(u8 USARTport, char *st);

void delay(u32 count)
{
    u32 i;
    for(i = 0; i < count; i++);
}

void clocktree_init(void)
{
    u8 PLL=7;
    unsigned char temp = 0;
    RCC->CR |= 0x00010000;
    while(!(RCC->CR>>17));
    RCC->CFGR = 0x00000400;
    RCC->CFGR &= ~(7 << 11);
    RCC->CFGR |= PLL << 18;
    RCC->CFGR |= 1 << 16;
    FLASH->ACR |= 0x32;
    RCC->CR |= 0x01000000;
    while(!(RCC->CR>>25));
    RCC->CFGR |= 0x00000002;
    while(temp != 0x02)
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}

void usart2_init(u32 pclk1, u32 baud)
{
    float temp;
    u16 mantissa;
    u16 fraction;
    temp = (float)(pclk1 * 1000000) / (baud * 16);
    mantissa = temp;
    fraction = (temp - mantissa) * 16;
			mantissa <<= 4;
    mantissa += fraction;
    RCC->APB2ENR |= 1 << 2;
    RCC->APB1ENR |= 1 << 17;
    GPIOA->CRL &= 0xFFFF00FF;
    GPIOA->CRL |= 0x00008B00;
    RCC->APB1RSTR |= 1 << 17;
    RCC->APB1RSTR &= ~(1 << 17);
    USART2->BRR = mantissa;
    USART2->CR1 |= 0x2008;
}

void usart1_init(u32 pclk2, u32 baud)
{
    float temp;
    u16 mantissa;
    u16 fraction;
    temp = (float)(pclk2 * 1000000) / (baud * 16);
    mantissa = temp;
    fraction = (temp - mantissa) * 16;
			mantissa <<= 4;
    mantissa += fraction;
    RCC->APB2ENR |= 1 << 14;
    RCC->APB2ENR |= 1 << 2;
    GPIOA->CRH &= 0xFFFFF00F;
    GPIOA->CRH |= 0x000008B0;
    RCC->APB2RSTR |= 1 << 14;
    RCC->APB2RSTR &= ~(1 << 14);
    USART1->BRR = mantissa;
    USART1->CR1 |= 0x2008;
}

void usart_print(u8 USARTport, char *st)
{
	u8 i=0;
	while (st[i] !=0x00)
	{
		if(USARTport == 1) USART1->DR = st[i];
		if(USARTport == 2) USART2->DR = st[i];
		delay(50000);
		if (i == 255) break;
		i++;
	}
}

int main(void)
{
    char my_cuid[] = "1155213082\r\n";
    clocktree_init();
    usart2_init(36, 9600);
    usart1_init(72, 9600);

    delay(1000000);

		usart_print(1, my_cuid);

    while(1)
    {
        
    }
}
