#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_LED.h"

void io_init(void) {
    RCC->APB2ENR |= 1 << 3;
    GPIOB->CRL &= ~(0xF << (5 * 4));
    GPIOB->CRL |= (0x3 << (5 * 4));
    GPIOB->BSRR = 1 << 5;
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
    GPIOB->BRR = 1 << 5;
    GPIOB->BSRR = 1 << 5;
    GPIOB->BRR = 1 << 5;
    GPIOB->BSRR = 1 << 5;
    GPIOB->ODR ^= 1 << 5;
    GPIOB->ODR ^= 1 << 5;
    GPIOB->ODR ^= 1 << 5;
    GPIOB->ODR ^= 1 << 5;
    GPIOB->ODR &= ~(1<<5);
    GPIOB->ODR |= 1<<5;
    GPIOB->ODR &= ~(1<<5);
    GPIOB->ODR |= 1<<5;
    TIM3->SR &= ~(1<<0);
    TIM3->SR &= ~(1<<0);
}

int main(void)
{
    clocktree_init();
    io_init();
    NVIC_SetPriorityGrouping(5);
    tim3_init(4999, 7199);

    while(1)
    {
    }
}
