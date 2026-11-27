#include "stm32f10x.h"
#include "IERG3810_Clock.h" 
#include "IERG3810_USART.h" 

int main(void)
{
    char my_cuid[] = "1155213082\r\n"; 

    clocktree_init();
    usart2_init(36, 9600);
    usart1_init(72, 9600);

    delay(1000000);

    usart_print_txe(1, my_cuid);

    while(1)
    {

    }
}
