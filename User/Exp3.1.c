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

    u16 colors[5] = {c_black, c_white, c_GREEN, c_RED, c_BLUE};
    
    u16 i, j;

    for (i = 0; i < 5; i++)
    {
  
        u16 start_x = 10 + i * 10;
        
        for (j = 0; j < 100; j++)
        {
            lcd_drawDot(start_x, 10 + j, colors[i]);
        }
    }
    
		delay(5000000); 

		
    lcd_drawDot(0, 0, c_YELLOW);
		
		
    delay(5000000); 

		for(int i = 0; i < 100; i++){
    lcd_drawDot(20+i, 0, c_GREEN);
		}
		
    delay(5000000);
		
		for(int i = 0; i < 100; i++){
    lcd_drawDot(0, 20+i, c_RED);
		}
		
    while(1)
    {
			
    }
}
