#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"

#define LED0_PWM_VAL TIM3->CCR2

volatile u32 heartbeat[10];
u16 led0pwmval = 0;
u8 dir = 1;

void tim3_init_pwm(u16 arr, u16 psc);
void tim4_init(u16 arr, u16 psc);
void systick_init_10ms(void);

int main(void)
{
    clocktree_init();
    io_init();
	  systick_init_10ms();
	
    tim3_init_pwm(9999, 47);
	  tim4_init(1249, 7199);

    heartbeat[0] = 2;

    while(1)
    {
        if (heartbeat[0] == 1)
        {
            heartbeat[0] = 2;
            if (dir) led0pwmval++;
            else led0pwmval--;
            if (led0pwmval > 5000) dir = 0;
            if (led0pwmval == 0) dir = 1;
            LED0_PWM_VAL = led0pwmval;
        }
    }
}

void tim3_init_pwm(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 3;
    GPIOB->CRL &= ~0xFF0FFFFF;
    GPIOB->CRL |= 0x00B00000;
    RCC->APB2ENR |= 1 << 0;
    AFIO->MAPR &= ~0xFFFFF3FF;
    AFIO->MAPR |= 1 << 11;
    RCC->APB1ENR |= 1 << 1;
    TIM3->ARR = arr;
    TIM3->PSC = psc;
    TIM3->CCMR1 |= 7 << 12;
    TIM3->CCMR1 |= 1 << 11;
    TIM3->CCER |= 1 << 4;
    TIM3->CR1 = 0x0080;
    TIM3->CR1 |= 0x01;
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
    if (TIM4->SR & (1<<0))
    {
        GPIOE->ODR ^= 1<<5;
    }
    TIM4->SR &= ~(1<<0);
}

void systick_init_10ms(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 89999;
    SysTick->CTRL = 0x03;
}
