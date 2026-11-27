#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"
#include "FONT.H"
#include "CFONT.H"

extern void delay(u32 count);

static void lcd_9341_setParameter(void);
static void lcd_wr_reg(u16 regval);
static void lcd_wr_data(u16 data);
static void lcd_backlight_init(void);

#define LCD_Light_ON (GPIOB->ODR |= (1 << 0))

void lcd_drawRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y)
{
    // Draw top line
    lcd_fillRectangle(color, start_x, length_x, start_y, 1);
    // Draw bottom line
    lcd_fillRectangle(color, start_x, length_x, start_y + length_y - 1, 1);
    // Draw left line
    lcd_fillRectangle(color, start_x, 1, start_y, length_y);
    // Draw right line
    lcd_fillRectangle(color, start_x + length_x - 1, 1, start_y, length_y);
}

void lcd_init(void)   //set FSMC
{
    RCC->AHBENR|=1<<8;          //FSMC
    RCC->APB2ENR|=1<<3;         //PORTB
    RCC->APB2ENR|=1<<5;         //PORTD
    RCC->APB2ENR|=1<<6;         //PORTE
    RCC->APB2ENR|=1<<8;         //PORTG
    GPIOB->CRL&=0XFFFFFFF0;     //PB0
    GPIOB->CRL|=0X00000003;
    //PORTD
    GPIOD->CRH&=0X00FFF000;
    GPIOD->CRH|=0XBB000BBB;
    GPIOD->CRL&=0XFF00FF00;
    GPIOD->CRL|=0X00BB00BB;
    //PORTE
    GPIOE->CRH&=0X00000000;
    GPIOE->CRH|=0XBBBBBBBB;
    GPIOE->CRL&=0X0FFFFFFF;
    GPIOE->CRL|=0XB0000000;
    //PORTG12
    GPIOG->CRH&=0XFFF0FFFF;
    GPIOG->CRH|=0X000B0000;
    GPIOG->CRL&=0XFFFFFFF0;     //PG0->RS
    GPIOG->CRL|=0X0000000B;

    // LCD uses FSMC Bank 4 memory bank.
    // Use Mode A
    FSMC_Bank1->BTCR[6]=0X00000000;  //FSMC_BCR4 (reset)
    FSMC_Bank1->BTCR[7]=0X00000000;  //FSMC_BTR4 (reset)
    FSMC_Bank1E->BWTR[6]=0X00000000; //FSMC_BWTR4 (reset)
    FSMC_Bank1->BTCR[6]|=1<<12;      //FSMC_BCR4 -> WREN
    FSMC_Bank1->BTCR[6]|=1<<14;      //FSMC_BCR4 -> EXTMOD
    FSMC_Bank1->BTCR[6]|=1<<4;       //FSMC_BCR4 -> MWID
    FSMC_Bank1->BTCR[7]|=0<<28;      //FSMC_BTR4 -> ACCMOD
    FSMC_Bank1->BTCR[7]|=1<<0;       //FSMC_BTR4 -> ADDSET
    FSMC_Bank1->BTCR[7]|=0XF<<8;     //FSMC_BTR4 -> DATAST
    FSMC_Bank1E->BWTR[6]|=0<<28;     //FSMC_BWTR4 -> ACCMOD
    FSMC_Bank1E->BWTR[6]|=0<<0;      //FSMC_BWTR4 -> ADDSET
    FSMC_Bank1E->BWTR[6]|=3<<8;      //FSMC_BWTR4 -> DATAST
    FSMC_Bank1->BTCR[6]|=1<<0;       //FSMC_BCR4 -> MBKEN

		delay_ms(50);

    //-- either one, check the label on the LCD
    lcd_9341_setParameter();

    lcd_backlight_init();   //students write this function, PB0
    LCD_Light_ON;           //students write this function, PB0
}

void lcd_9341_setParameter(void)
{
    lcd_wr_reg(0X01);   //Software reset
    lcd_wr_reg(0X11);   //Exit sleep_mode
    lcd_wr_reg(0X3A);   //Set_pixel_format
    lcd_wr_data(0X55);  //65536 colors
    lcd_wr_reg(0X29);   //Display ON
    lcd_wr_reg(0X36);   //Memory Access Control
    lcd_wr_data(0XC8);  //control Display direction
}

void lcd_wr_reg(u16 regval)
{
    LCD->LCD_REG=regval;
}

void lcd_wr_data(u16 data)
{
    LCD->LCD_RAM=data;
}

void lcd_drawDot(u16 x, u16 y, u16 color)
{
    lcd_wr_reg(0x2A);   //set x position
			lcd_wr_data(x>>8);
			lcd_wr_data(x & 0xFF);
			lcd_wr_data(0x01);
			lcd_wr_data(0x3F);
    lcd_wr_reg(0x2B);   //set y position
			lcd_wr_data(y>>8);
			lcd_wr_data(y & 0xFF);
			lcd_wr_data(0x01);
			lcd_wr_data(0xDF);
    lcd_wr_reg(0x2C);   //set point with color
    lcd_wr_data(color);
}

static void lcd_backlight_init(void)
{

}

void lcd_fillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y)
{
    u32 index=0;
    lcd_wr_reg(0x2A);
    lcd_wr_data(start_x>>8);
    lcd_wr_data(start_x & 0xFF);
    lcd_wr_data((length_x + start_x - 1) >> 8);
    lcd_wr_data((length_x + start_x - 1) & 0xFF);
    lcd_wr_reg(0x2B);
    lcd_wr_data(start_y>>8);
    lcd_wr_data(start_y & 0xFF);
    lcd_wr_data((length_y + start_y - 1) >> 8);
    lcd_wr_data((length_y + start_y - 1) & 0xFF);
    lcd_wr_reg(0x2C); //LCD_WriteRAM_Prepare();
    delay(100);
    for(index=0; index<length_x * length_y; index++)
    {
        lcd_wr_data(color);
    }
}

static void lcd_draw_segment_a(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x + 10, 55, start_y - 10, 10);
}

static void lcd_draw_segment_b(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x + 65, 10, start_y - 65, 55);
}

static void lcd_draw_segment_c(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x + 65, 10, start_y - 130, 55);
}

static void lcd_draw_segment_d(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x + 10, 55, start_y - 140, 10);
}

static void lcd_draw_segment_e(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x, 10, start_y - 130, 55);
}

static void lcd_draw_segment_f(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x, 10, start_y - 65, 55);
}

static void lcd_draw_segment_g(u16 color, u16 start_x, u16 start_y) {
    lcd_fillRectangle(color, start_x + 10, 55, start_y - 75, 10);
}

void lcd_sevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit)
{
    switch(digit)
    {
        case 0: 
            lcd_draw_segment_a(color, start_x, start_y);
            lcd_draw_segment_b(color, start_x, start_y);
            lcd_draw_segment_c(color, start_x, start_y);
            lcd_draw_segment_d(color, start_x, start_y);
            lcd_draw_segment_e(color, start_x, start_y);
            lcd_draw_segment_f(color, start_x, start_y);
            break;
        case 1: 
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            break;
        case 2: 
            lcd_draw_segment_a(color, start_x, start_y);
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_d(color, start_x, start_y); 
            lcd_draw_segment_e(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            break;
        case 3: 
            lcd_draw_segment_a(color, start_x, start_y); 
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            lcd_draw_segment_d(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            break;
        case 4: 
            lcd_draw_segment_f(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            break;
        case 5: 
            lcd_draw_segment_a(color, start_x, start_y); 
            lcd_draw_segment_f(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            lcd_draw_segment_d(color, start_x, start_y); 
            break;
        case 6: 
						lcd_draw_segment_a(color, start_x, start_y); 
            lcd_draw_segment_e(color, start_x, start_y); 
            lcd_draw_segment_d(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            lcd_draw_segment_f(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            break;
        case 7: 
            lcd_draw_segment_a(color, start_x, start_y); 
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            break;
        case 8: 
            lcd_draw_segment_a(color, start_x, start_y);
            lcd_draw_segment_b(color, start_x, start_y);
            lcd_draw_segment_c(color, start_x, start_y);
            lcd_draw_segment_d(color, start_x, start_y);
            lcd_draw_segment_e(color, start_x, start_y);
            lcd_draw_segment_f(color, start_x, start_y);
            lcd_draw_segment_g(color, start_x, start_y);
            break;
        case 9: 
            lcd_draw_segment_a(color, start_x, start_y); 
            lcd_draw_segment_f(color, start_x, start_y); 
            lcd_draw_segment_c(color, start_x, start_y); 
            lcd_draw_segment_d(color, start_x, start_y); 
            lcd_draw_segment_b(color, start_x, start_y); 
            lcd_draw_segment_g(color, start_x, start_y); 
            break;
        default:
            break;
    }
}

void lcd_showChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor)
{
    u8 i, b, temp1, temp2;
    u16 tempX, tempY;
    if (ascii < 32 || ascii > 127) return; 
    ascii -= 32;
    tempX = x;
    for (i = 0; i < 16; i = i + 2)
    {
        temp1 = asc2_1608[ascii][i];
        temp2 = asc2_1608[ascii][i+1];
        tempY = y;
        for (b = 0; b < 8; b++)
        {
            //if (temp1 % 2 == 1) lcd_drawDot(tempX, tempY + 8, color);
            //if (temp2 % 2 == 1) lcd_drawDot(tempX, tempY, color);
					
					if (temp2 % 2 == 1) {
                lcd_drawDot(tempX, tempY, color);
            } else {
                lcd_drawDot(tempX, tempY, bgcolor);
            }
        
            if (temp1 % 2 == 1) {
                lcd_drawDot(tempX, tempY + 8, color);
            } else {
                lcd_drawDot(tempX, tempY + 8, bgcolor);
            }
					
            temp1 = temp1 >> 1;
            temp2 = temp2 >> 1;
            tempY++;
        }
        tempX++;
    }
}

void lcd_showChinChar(u16 x, u16 y, u8 index, u16 color, u16 bgcolor)
{
    u8 row, bit_idx;
    u8 data_left, data_right;
    u16 dest_y;

    if (index >= 10) return;

    for (row = 0; row < 16; row++)
    {
        dest_y = y + (15 - row);

        data_left  = chi_1616[index][row * 2];
        data_right = chi_1616[index][row * 2 + 1];

        for (bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            if (data_left & 0x80) 
            {
                lcd_drawDot(x + bit_idx, dest_y, color);
            }
            else
            {
                lcd_drawDot(x + bit_idx, dest_y, bgcolor);
            }
            data_left <<= 1;
        }

        for (bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            if (data_right & 0x80)
            {
                lcd_drawDot(x + 8 + bit_idx, dest_y, color);
            }
            else
            {
                lcd_drawDot(x + 8 + bit_idx, dest_y, bgcolor);
            }
            data_right <<= 1;
        }
    }
}
