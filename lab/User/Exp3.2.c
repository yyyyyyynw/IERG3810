#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"

void delay(u32 count)
{
    volatile u32 i;
    for(i = 0; i < count; i++);
}

int main(void)
{
    const u16 screen_width = 240;
    const u16 screen_height = 320;

    const u16 box_size = 50;

    u16 box_start_x = (screen_width - box_size) / 2;  
    u16 box_start_y = (screen_height - box_size) / 2; 

    delay(1000000);

    lcd_init();

    lcd_fillRectangle(c_YELLOW, 0, screen_width, 0, screen_height);

    lcd_fillRectangle(c_BLUE, box_start_x, box_size, box_start_y, box_size);

    while(1)
    {

    }
}
