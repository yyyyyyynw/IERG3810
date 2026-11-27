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
		
    for (i = 0; i < 10; i++) {
        lcd_showChar(30 + i * 8, 100, cuid1[i], c_BLUE, c_black);
    }
		
    u8 name1_indices[] = {0, 1, 2}; 
    u16 name_start_x = 30 + 10 * 8 + 10; 

    for (i = 0; i < 3; i++) { 
        lcd_showChinChar(name_start_x + i * 16, 100, name1_indices[i], c_BLUE, c_black);
    }

    for (i = 0; i < 10; i++) {
        lcd_showChar(30 + i * 8, 140, cuid2[i], c_RED, c_black);
    }

    u8 name2_indices[] = {3, 4, 5}; 
    for (i = 0; i < 3; i++) { 
        lcd_showChinChar(name_start_x + i * 16, 140, name2_indices[i], c_RED, c_black);
    }
    while(1)
    {
			
    }
}
