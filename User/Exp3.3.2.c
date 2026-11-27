#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"

void delay(u32 count)
{
    volatile u32 i;
    for(i = 0; i < count; i++);
}

int main(void)
{
    lcd_init();

    delay(1000000); 

    lcd_fillRectangle(c_YELLOW, 0, 240, 0, 320);

    lcd_sevenSegment(c_black, 82, 230, 8);

    while(1)
    {
    
    }
}
