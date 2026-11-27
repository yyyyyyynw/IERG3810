#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"
#include "IERG3810_LED.h"

#define LED0_PWM_VAL TIM3->CCR2

u16 led0pwmval = 0;
u8 dir = 1;
u32 heartbeat[10];

void delay(u32 count)
{
		volatile u32 i;
		for(i = 0;i < count;i++);
}

void tim3_init_pwm(u16 arr, u16 psc);

int main(void)
{
    clocktree_init();
    io_init();
    tim3_init_pwm(9999, 72);

    while(1)
    {
			delay(1500);
			if (dir) led0pwmval++;
			else led0pwmval--;
			if (led0pwmval > 5000) dir=0;
			if (led0pwmval ==0) dir=1;
			LED0_PWM_VAL = led0pwmval;
    }
}

void tim3_init_pwm(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 3;
    GPIOB->CRL &= 0xFF0FFFFF;
    GPIOB->CRL |= 0x00B00000;
    RCC->APB2ENR |= 1 << 0;
    AFIO->MAPR &= 0xFFFFF3FF;
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
