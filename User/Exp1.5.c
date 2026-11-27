#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"

void Delay(u32 count)
{
    u32 i;
    for(i = 0; i < count; i++);
}

int main(void)
{
    IERG3810_LED_Init();
    IERG3810_Buzzer_Init();
    IERG3810_KEY_Init();
    u8 led1_state = 0; 

    while (1)
    {
        if (KEY2_Read() == 0) 
        {
            DS0_On();
        }
        else 
        {
            DS0_Off();
        }

        if (KEY1_Read() == 0) 
        {
            Delay(10000); 
            if (KEY1_Read() == 0) 
            {
                if (led1_state == 0) {
                    DS1_On();
                    led1_state = 1;
                } else {
                    DS1_Off();
                    led1_state = 0;
                }
                while(KEY1_Read() == 0);
            }
        }

        if (KEYUP_Read() == 1) 
        {
            Delay(10000); 
            if (KEYUP_Read() == 1) 
            {
                Buzzer_Toggle(); 
                while(KEYUP_Read() == 1);
            }
        }
    }
}
