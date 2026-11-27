#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"
#include "IERG3810_LED.h"

void tim3_init(u16 arr, u16 psc);
void tim4_init(u16 arr, u16 psc);

int main(void)
{
    clocktree_init();
    io_init();
    NVIC_SetPriorityGrouping(5);
    tim3_init(4999, 7199);
    tim4_init(1249, 7199);
    DS0_Off();
    DS1_Off();

    while(1)
    {
    }
}

void tim3_init(u16 arr, u16 psc)
{
    RCC->APB1ENR|=1<<1;
    TIM3->ARR=arr;
    TIM3->PSC=psc;
    TIM3->DIER|=1<<0;
    TIM3->CR1|=0x01;
    NVIC->IP[29] = 0x45;
    NVIC->ISER[0] |= (1<<29);
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & 1<<0)
    {
        GPIOB->ODR ^= 1<<5;
    }
    TIM3->SR &= ~(1<<0);
}

void tim4_init(u16 arr, u16 psc)
{
    RCC->APB1ENR|=1<<2;
    TIM4->ARR=arr;
    TIM4->PSC=psc;
    TIM4->DIER|=1<<0;
    TIM4->CR1|=0x01;
    NVIC->IP[30] = 0x45;
    NVIC->ISER[0] |= (1<<30);
}

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 1<<0)
    {
        GPIOE->ODR ^= 1<<5;
    }
    TIM4->SR &= ~(1<<0);
}
