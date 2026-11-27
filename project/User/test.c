#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_LED.h"
#include "IERG3810_io.h"
#include "Board.h"
#include "JoyPad.h"
#include <stdlib.h>

#define JOYPAD_DPAD_MASK   (JOYPAD_KEY_UP | JOYPAD_KEY_DOWN | JOYPAD_KEY_LEFT | JOYPAD_KEY_RIGHT)
#define JOYPAD_ACTION_MASK (JOYPAD_KEY_START | JOYPAD_KEY_SELECT | JOYPAD_KEY_A | JOYPAD_KEY_B)

int menu_needs_redraw         = 1;
int instructions_needs_redraw = 1;

volatile int game_tick_ready        = 0;
volatile int led_flash_counter      = 0;
volatile int endgame_led_timer      = 0;
volatile int endgame_led_mode       = 0;
volatile int endgame_toggle_counter = 0;

volatile uint8_t joypad_current = 0;
volatile uint8_t joypad_last    = 0;
volatile uint8_t joypad_pressed = 0;

void Hardware_Init(void);
void Timer_Init(void);
void Game_HandleInput(void);
void JoyPad_Poll(void);

int main(void)
{
    Hardware_Init();
    Starfield_Init();
    current_state = START_SCREEN;

    while (1)
    {
        if (current_state == PLAYING) {
            Game_HandleInput();
        }

        switch (current_state)
        {
        case START_SCREEN:
            Game_DrawStartScreen();
            while (current_state == START_SCREEN)
            {
                Starfield_Update();
                delay_ms(10);

                if ((joypad_pressed & JOYPAD_KEY_SELECT) &&
                    !(joypad_pressed & (JOYPAD_KEY_A | JOYPAD_KEY_B)))
                {
                    current_state = INSTRUCTIONS;
                    instructions_needs_redraw = 1;
                    joypad_pressed = 0;
                }
            }
            break;

        case INSTRUCTIONS:
            if (instructions_needs_redraw) {
                Game_DrawInstructions();
                instructions_needs_redraw = 0;
            }
            while (current_state == INSTRUCTIONS)
            {
                Starfield_Update();
                delay_ms(10);

                if ((joypad_pressed & JOYPAD_KEY_SELECT) &&
                    !(joypad_pressed & (JOYPAD_KEY_A | JOYPAD_KEY_B)))
                {
                    current_state = MENU;
                    menu_needs_redraw = 1;
                    joypad_pressed = 0;
                }
            }
            break;

        case MENU:
            if (menu_needs_redraw) {
                Game_DrawMenu();
                menu_needs_redraw = 0;
            }

            while (current_state == MENU)
            {
                Starfield_Update();
                delay_ms(10);

                if ((joypad_pressed & (JOYPAD_KEY_START | JOYPAD_KEY_SELECT)) &&
                    !(joypad_pressed & (JOYPAD_KEY_A | JOYPAD_KEY_B)))
                {
                    current_level_index = selected_level_index;
                    Game_Init();
                    current_state = PLAYING;
                    joypad_pressed = 0;
                }
                else if ((joypad_current & JOYPAD_KEY_UP) &&
                         !(joypad_current & JOYPAD_KEY_DOWN))
                {
                    if (selected_level_index > 0) {
                        selected_level_index--;
                        Game_DrawMenuList();
                    }
                }
                else if ((joypad_current & JOYPAD_KEY_DOWN) &&
                         !(joypad_current & JOYPAD_KEY_UP))
                {
                    if (selected_level_index < NUM_LEVELS - 1) {
                        selected_level_index++;
                        Game_DrawMenuList();
                    }
                }
            }
            break;

        case PLAYING:
            if (game_needs_full_redraw) {
                Game_RedrawScreen();
                game_needs_full_redraw = 0;
            }
            if (game_tick_ready) {
                Game_Update();
                Game_Draw();
                game_tick_ready = 0;
            }
            break;
        
        case GAME_OVER:
            Update_HighScore();
            endgame_led_mode  = 2;
            endgame_led_timer = 300;
            Game_DrawGameOverScreen();
            while (current_state == GAME_OVER)
            {
                delay_ms(10);
                if ((joypad_pressed & JOYPAD_KEY_SELECT) &&
                    !(joypad_pressed & (JOYPAD_KEY_A | JOYPAD_KEY_B)))
                {
                    current_state = MENU;
                    menu_needs_redraw = 1;
                    joypad_pressed = 0;
                }
            }
            break;

        case GAME_WON:
            Update_HighScore();
            endgame_led_mode  = 1;
            endgame_led_timer = 300;
            Game_DrawGameWonScreen();
            while (current_state == GAME_WON)
            {
                delay_ms(10);
                if ((joypad_pressed & JOYPAD_KEY_SELECT) &&
                    !(joypad_pressed & (JOYPAD_KEY_A | JOYPAD_KEY_B)))
                {
                    current_state = MENU;
                    menu_needs_redraw = 1;
                    joypad_pressed = 0;
                }
            }
            break;
        }
    }
}

void JoyPad_Poll(void)
{
    joypad_last    = joypad_current;
    joypad_current = JoyPad_Read();
    joypad_pressed = joypad_current & ~joypad_last;
}

void Game_HandleInput(void)
{
    
    if (!direction_changed_this_tick &&
        (joypad_current & JOYPAD_DPAD_MASK) &&
        !(joypad_current & JOYPAD_ACTION_MASK))
    {
        if (joypad_current & JOYPAD_KEY_UP)
        {
            if (snake.direction.y == 0) {
                snake.direction.x = 0;
                snake.direction.y = 1;
                direction_changed_this_tick = 1;
            }
        }
        else if (joypad_current & JOYPAD_KEY_DOWN)
        {
            if (snake.direction.y == 0) {
                snake.direction.x = 0;
                snake.direction.y = -1;
                direction_changed_this_tick = 1;
            }
        }
        else if (joypad_current & JOYPAD_KEY_LEFT)
        {
            if (snake.direction.x == 0) {
                snake.direction.x = -1;
                snake.direction.y = 0;
                direction_changed_this_tick = 1;
            }
        }
        else if (joypad_current & JOYPAD_KEY_RIGHT)
        {
            if (snake.direction.x == 0) {
                snake.direction.x = 1;
                snake.direction.y = 0;
                direction_changed_this_tick = 1;
            }
        }
    }
}

void Hardware_Init(void)
{
    clocktree_init();
    io_init();
    lcd_init();
    JoyPad_Init();
    IERG3810_Buzzer_Init();
    Timer_Init();
    srand(TIM3->CNT);
}

void Timer_Init(void)
{
    RCC->APB1ENR |= 1 << 1;
    TIM3->PSC = 7199;
    TIM3->ARR = 99;
    TIM3->DIER |= 1 << 0;
    TIM3->CR1  |= 1 << 0;
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void)
{
    static int speed_accum = 0;
    int toggle_period = 0;

    if (TIM3->SR & 0x01)
    {
        TIM3->SR &= ~0x01;

        JoyPad_Poll();

        if (led_flash_counter > 0) {
            DS1_On();
            led_flash_counter -= 10;
        } else {
            DS1_Off();
            led_flash_counter = 0;
        }

        if (endgame_led_timer > 0) {
            endgame_led_timer--;
            toggle_period = 0;
            if (endgame_led_mode == 1) toggle_period = 10;
            else if (endgame_led_mode == 2) toggle_period = 25;

            if (toggle_period > 0) {
                endgame_toggle_counter++;
                if (endgame_toggle_counter >= toggle_period) {
                    GPIOB->ODR ^= (1 << 5);
                    endgame_toggle_counter = 0;
                }
            }
        } else {
            if (endgame_led_mode != 0) {
                DS0_Off();
                endgame_led_mode = 0;
            }
        }

        if (current_state == PLAYING) {
            speed_accum += 10;
            if (speed_accum >= game_speed) {
                game_tick_ready = 1;
                speed_accum = 0;
            }
        } else {
            speed_accum = 0;
        }
    }
}
