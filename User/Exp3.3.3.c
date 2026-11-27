#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"

void delay(u32 count)
{
    volatile u32 i;
    for(i = 0; i < count; i++);
}

int main(void)
{
   
    delay(1000000); 
	
		lcd_init();
	
    lcd_fillRectangle(c_black, 0, 240, 0, 320);

    while(1)
    {
				for (int i = 9; i >= 0; i--)
        {
            
            lcd_sevenSegment(c_BLUE, 80, 240, i);

            delay(10000000);
						
						lcd_sevenSegment(c_black, 80, 240, i);
        }
    }
}
