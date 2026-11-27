#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"
#include "IERG3810_LED.h"

volatile u32 heartbeat[10];

void systick_init_10ms(void);

int main(void)
{
    clocktree_init();
    io_init();
    systick_init_10ms();
    
    heartbeat[0]= 10;
    heartbeat[1]= 17;
    
    while(1)
    {
        if (heartbeat[0] == 1)
        {
            heartbeat[0] = 10;
            GPIOB->ODR ^= 1<<5;
        }
        if (heartbeat[1] == 1)
        {
            heartbeat[1] = 17;
            GPIOE->ODR ^= 1<<5;
        }
    }
}

void systick_init_10ms(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 89999;
    SysTick->CTRL = 0x03;
}
