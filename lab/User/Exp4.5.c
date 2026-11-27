#include "stm32f10x.h"

#define DS0_on      (GPIOB->BSRR = (1 << (5 + 16))) 
#define DS0_off     (GPIOB->BSRR = (1 << 5))        
#define DS1_on      (GPIOE->BSRR = (1 << (5 + 16))) 
#define DS1_off     (GPIOE->BSRR = (1 << 5))        

volatile u32 sheep = 0;
volatile u8 ps2count = 0;      
volatile u8 ps2scancode = 0;   
u8 ps2_is_release = 0;  

void delay(u32 count)
{
	volatile u32 i;
	for(i = 0;i < count;i++);
}

void key2_extiInit(void)
{
    RCC->APB2ENR |= 1<<6;
    GPIOE->CRL &= 0XFFFFF0FF;
    GPIOE->CRL |= 0X00000800;
    GPIOE->ODR |= 1 << 2;
    RCC->APB2ENR |= 0x01;
    AFIO->EXTICR[0] &= 0xFFFFF0FF;
    AFIO->EXTICR[0] |= 0x00000400;
    EXTI->IMR |= 1<<2;
    EXTI->RTSR |= 1<<2;
    
    NVIC->IP[8] = 0x65;
    
    NVIC->ISER[0] |= (1<<8);
}

void nvic_setPriorityGroup(u8 priGroup)
{
    u32 tmp1, tmp2;
    tmp2 = (priGroup & 0x00000007) << 8;
    tmp1 = SCB->AIRCR &= 0x0000F8FF;
    tmp1 |= 0x05FA0000;
    SCB->AIRCR = tmp1 |= tmp2;
}

void EXTI2_IRQHandler(void)
{
    u8 i;
    for (i=0; i<10; i++)
    {
        DS0_on;
        delay(1000000);
        DS0_off;
        delay(1000000);
    }
    EXTI->PR = 1<<2;
}

void io_init(void) 
{
    RCC->APB2ENR |= (1 << 3);
    RCC->APB2ENR |= (1 << 6); 

    GPIOB->CRL &= 0xFF0FFFFF; 
    GPIOB->CRL |= 0x00300000; 

    GPIOE->CRL &= 0xFF0FFFFF;
    GPIOE->CRL |= 0x00300000;
}

void clocktree_init(void) {

}

void keyup_extiInit(void)
{
    RCC->APB2ENR |= (1 << 2); 
    RCC->APB2ENR |= (1 << 0); 

    GPIOA->CRL &= 0XFFFFFFF0; 
    GPIOA->CRL |= 0X00000008;
    GPIOA->ODR &= ~(1 << 0); 

    AFIO->EXTICR[0] &= 0xFFFFFFF0; 

    EXTI->IMR |= (1 << 0);  
    EXTI->RTSR |= (1 << 0);  

    NVIC->IP[6] = 0x35;     
    NVIC->ISER[0] |= (1 << 6); 
}

void EXTI0_IRQHandler(void)
{
    u8 i;
    if(EXTI->PR & (1 << 0))
    {
        for (i=0; i<10; i++)
        {
            DS1_on;
            delay(1000000);
            DS1_off;
            delay(1000000);
        }
        EXTI->PR = (1 << 0);
    }
}

void pskey_extiInit(void)
{
    RCC->APB2ENR |= (1 << 4); 
    RCC->APB2ENR |= (1 << 0); 

    GPIOC->CRH &= 0xFFFF00FF; 
    GPIOC->CRH |= 0x00004400;

    AFIO->EXTICR[2] &= 0xFFFF0FFF; 
    AFIO->EXTICR[2] |= 0x00002000;

    EXTI->IMR |= (1 << 11);  
    EXTI->FTSR |= (1 << 11); 
    EXTI->RTSR &= ~(1 << 11);

    NVIC->IP[40] = 0x55; 
    NVIC->ISER[1] |= (1 << (40 - 32)); 
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & (1 << 11))
	{
		static u16 temp_ps2_data = 0; 
		u8 data_bit = (GPIOC->IDR >> 10) & 0x01; 

		if (ps2count == 0) {
			temp_ps2_data = 0;
		}

		if (ps2count < 11) {
			temp_ps2_data |= (data_bit << ps2count); 
			ps2count++;
		}

		if (ps2count >= 11) {

			ps2scancode = (temp_ps2_data >> 1) & 0xFF;
			EXTI->IMR &= ~(1 << 11);
		}
		
	EXTI->PR = 1<<11;
	EXTI->PR = 1<<11;
	}
}

int main(void)
{
    io_init();
    clocktree_init();
    nvic_setPriorityGroup(5);
    key2_extiInit();
		keyup_extiInit();
		pskey_extiInit();
    DS0_off;
		DS1_off;
	
    while(1)
    {
			sheep++;
			if(ps2count >= 11)
			{
				if (ps2scancode == 0xF0) {
				ps2_is_release = 1;
			} else {
				if (ps2_is_release) {

					if (ps2scancode == 0x69) { 
						DS0_off;
					}
					if (ps2scancode == 0x72) { 
						DS1_off;
					}
					ps2_is_release = 0; 
				} else { 

					if (ps2scancode == 0x69) { 
						DS0_on;
					}
					if (ps2scancode == 0x72) { 
						DS1_on;
					}
				}
			}

			ps2count = 0;
			EXTI->IMR |= (1<<11);
			}
    }
}
