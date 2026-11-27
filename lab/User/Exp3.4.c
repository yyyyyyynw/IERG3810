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

    lcd_fillRectangle(c_black, 0, 240, 0, 320);

    char cuid1[] = "1155213082";
    char cuid2[] = "1155212799";
	
    u16 i;

    u16 start_x = 80;

    for (i = 0; i < 10; i++) {
        lcd_showChar(start_x + i * 8, 100, cuid1[i], c_BLUE, c_black);
    }

    for (i = 0; i < 10; i++) {
        lcd_showChar(start_x + i * 8, 120, cuid2[i], c_RED, c_black);
    }

    while(1)
    {
			
    }
}
