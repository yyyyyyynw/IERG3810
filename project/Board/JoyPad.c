#include "stm32f10x.h"
#include "JoyPad.h"

void JoyPad_Delay(uint32_t xus)
{
	for(; xus > 0; xus--)
	{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

void JoyPad_Init(void)
{
    RCC->APB2ENR |= 1 << 3;
    RCC->APB2ENR |= 1 << 5;

    GPIOB->CRH &= 0xFFF00FFF;
    GPIOB->CRH |= 0x00038000;
    
    GPIOD->CRL &= 0xFFFF0FFF;
    GPIOD->CRL |= 0x00003000;

    GPIOB->ODR |= (1 << 10);
    GPIOB->ODR &= ~(1 << 11);
    GPIOD->ODR &= ~(1 << 3);
}

uint8_t JoyPad_Read(void)
{
    uint8_t temp = 0;
    uint8_t i;

    GPIOB->ODR |= (1 << 11);
    JoyPad_Delay(12);
    GPIOB->ODR &= ~(1 << 11);
    JoyPad_Delay(12);

    for(i = 0; i < 8; i++)
    {
        if((GPIOB->IDR & (1 << 10)) == 0)
        {
            temp |= (1 << i);
        }

        GPIOD->ODR |= (1 << 3);
        JoyPad_Delay(12);
        GPIOD->ODR &= ~(1 << 3);
        JoyPad_Delay(12);
    }

    return temp;
}
