#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"

#define R_VAL TIM3->CCR1
#define G_VAL TIM3->CCR2
#define B_VAL TIM3->CCR3

u32 heartbeat[10];
u16 r=9999, g=0, b=0;
u8 state = 0;

void tim3_init_pwm_rgb(u16 arr, u16 psc);
void systick_init_10ms(void);
void procedure4(void);
void procedure5(void);

int main(void)
{
    clocktree_init();
    io_init();
    tim3_init_pwm_rgb(9999, 47);
    systick_init_10ms();
    
    heartbeat[0] = 2;

    while(1)
    {
        if (heartbeat[0] == 1)
        {
            heartbeat[0] = 2;
					
            procedure4();
        }
    }
}

void procedure4(void)
{
    u16 step = 50;
    switch(state)
    {
        case 0: g += step; if(g >= 9999) {g=9999; state=1;} break; // R -> Y
        case 1: r -= step; if(r <= 0)    {r=0;    state=2;} break; // Y -> G
        case 2: b += step; if(b >= 9999) {b=9999; state=3;} break; // G -> C
        case 3: g -= step; if(g <= 0)    {g=0;    state=4;} break; // C -> B
        case 4: r += step; if(r >= 9999) {r=9999; state=5;} break; // B -> M
        case 5: b -= step; if(b <= 0)    {b=0;    state=0;} break; // M -> R
    }
    R_VAL = r;
    G_VAL = g;
    B_VAL = b;
}

void procedure5(void)
{
    static u8 dark_state = 0;
    u16 step = 100;

    if (dark_state == 0) { // Color fade in/out
        switch(state) {
            case 0: r += step; if(r >= 9999) {r=9999; dark_state=1;} break; // Fading in R
            case 1: g += step; if(g >= 9999) {g=9999; dark_state=1;} break; // Fading in G
            case 2: b += step; if(b >= 9999) {b=9999; dark_state=1;} break; // Fading in B
            case 3: r += step; g += step; if(r >= 9999) {r=9999; g=9999; dark_state=1;} break; // Fading in Y
            case 4: g += step; b += step; if(g >= 9999) {g=9999; b=9999; dark_state=1;} break; // Fading in C
            case 5: r += step; b += step; if(r >= 9999) {r=9999; b=9999; dark_state=1;} break; // Fading in M
        }
    } else { // Color fade out
        r = (r > step) ? r - step : 0;
        g = (g > step) ? g - step : 0;
        b = (b > step) ? b - step : 0;
        if (r == 0 && g == 0 && b == 0) {
            dark_state = 0;
            state = (state + 1) % 6;
        }
    }
    R_VAL = r;
    G_VAL = g;
    B_VAL = b;
}

void tim3_init_pwm_rgb(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 4;
    GPIOC->CRL &= 0x00FFFFFF;
    GPIOC->CRL |= 0xBB000000;
    GPIOC->CRH &= 0xFFFFFFF0;
    GPIOC->CRH |= 0x0000000B;

    RCC->APB2ENR |= 1 << 0;
    AFIO->MAPR &= 0xFFFFC3FF;
    AFIO->MAPR |= 3 << 10;
    
    RCC->APB1ENR |= 1 << 1;
    TIM3->ARR = arr;
    TIM3->PSC = psc;
    
    TIM3->CCMR1 |= 6 << 4;
    TIM3->CCMR1 |= 1 << 3;
    TIM3->CCMR1 |= 6 << 12;
    TIM3->CCMR1 |= 1 << 11;
    
    TIM3->CCMR2 |= 6 << 4;
    TIM3->CCMR2 |= 1 << 3;
    
    TIM3->CCER |= 1 << 0;
    TIM3->CCER |= 1 << 4;
    TIM3->CCER |= 1 << 8;
    
    TIM3->CR1 = 0x0080;
    TIM3->CR1 |= 0x01;
}

void systick_init_10ms(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 89999;
    SysTick->CTRL = 0x03;
}
