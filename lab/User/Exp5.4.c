#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"
#include "IERG3810_LED.h"

#define DS0_on  (GPIOB->BRR = 1 << 5)
#define DS0_Off (GPIOB->BSRR = 1 << 5)

void tim3_init(u16 arr, u16 psc);
void ds0_turnOff(void);
void ds0_turnOff2(void);

int main(void)
{
    clocktree_init();
    io_init();
    NVIC_SetPriorityGrouping(5);
    tim3_init(4999, 7199);
		DS0_Off;

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

void ds0_turnOff(void)
{
    GPIOB->BSRR = 1 << 5;
}

void ds0_turnOff2(void)
{
    ds0_turnOff();
}

void TIM3_IRQHandler(void)
{
    DS0_on;
    DS0_Off;
    DS0_on;
    ds0_turnOff();
    DS0_on;
    ds0_turnOff2();
    DS0_on;
    TIM3->SR &= ~(1<<0);
    TIM3->SR &= ~(1<<0);
}
