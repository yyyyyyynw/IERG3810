#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_io.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"
#include "IERG3810_USART.h" // ?? delay() ?????
#include <stdlib.h> // ?? rand() ? srand()
#include <stdio.h>  // ?? sprintf()

// =================================================================================
// ????????? (Game Configuration and Constants)
// =================================================================================

#define GRID_SIZE 15
#define CELL_PIXEL_SIZE 16 // ???? 16x16 ??
#define MAX_SNAKE_LENGTH (GRID_SIZE * GRID_SIZE)
#define MAX_ROCKS 5
#define MAX_PORTALS 2

// ????? LCD ????? (???)
#define BOARD_START_X 0
#define BOARD_START_Y 0

// ????
#define BG_COLOR c_black
#define GRID_COLOR 0x31A6 // ?????
#define SNAKE_COLOR c_GREEN
#define FRUIT_COLOR c_RED
#define ROCK_COLOR 0x8410 // ????
#define PORTAL_COLOR_1 c_BLUE
#define PORTAL_COLOR_2 c_YELLOW
#define TEXT_COLOR c_white
#define HIGHLIGHT_COLOR c_YELLOW

// =================================================================================
// ?????? (Data Structures)
// =================================================================================

// ??????
typedef enum {
    STATE_START_MENU,
    STATE_IN_GAME,
    STATE_GAME_OVER,
    STATE_GAME_WIN
} GameState;

// ???
typedef struct {
    int x;
    int y;
} Point;

// ????
typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    Point direction; // x, y ????? -1, 0, ? 1
} Snake;

// =================================================================================
// ???? (Global Variables)
// =================================================================================

volatile u32 systick_count = 0; // SysTick ?????,??????

GameState current_state = STATE_START_MENU;
Snake snake;
Point fruit;
Point rocks[MAX_ROCKS];
Point portals[MAX_PORTALS];

int current_level = 1;
int selected_level_menu = 1; // ????????level
int score = 0;
int high_score = 0;
int num_fruits_to_win = 5;
int num_rocks = 0;
u32 game_speed_delay = 20; // ????????? SysTick ??,????????
u32 rock_move_delay = 200; // ???????
u32 last_move_time = 0;
u32 last_rock_move_time = 0;

// =================================================================================
// ?????? (Function Prototypes)
// =================================================================================

// SysTick Handler (!!??!! ???? stm32f10x_it.c)
// void SysTick_Handler(void);

// ????
void draw_cell(int x, int y, u16 color);
void draw_game_board(void);
void draw_snake(void);
void draw_all_items(void);
void update_info_display(void);
void draw_string(u16 x, u16 y, char* str, u16 color);

// ??????
void game_init(int level);
void place_item_randomly(Point* item);
void update_snake(void);
void handle_game_input(void);
void check_collisions(void);
void update_moving_rocks(void);

// ??????
void handle_start_menu(void);
void handle_in_game(void);
void handle_end_screen(u8 is_win);

// =================================================================================
// SysTick ?????? (!!??!! ???????? stm32f10x_it.c ???)
// =================================================================================
/*
// ? stm32f10x_it.c ?:
// extern volatile u32 systick_count;
// void SysTick_Handler(void) {
//     systick_count++;
// }
*/

// =================================================================================
// ??? (Main Function)
// =================================================================================

int main(void) {
    // ???????,????????? "Board" ???
    clocktree_init(); // ?? IERG3810_Clock.c
    io_init();        // ?? IERG3810_io.c, ????? LED, KEY, Buzzer
    lcd_init();       // ?? IERG3810_TFTLCD.c
    
    // ?? SysTick ? 10ms ???? (100Hz)
    // SystemCoreClock ? clocktree_init() ?? 72MHz
    SysTick_Config(SystemCoreClock / 100); 

    // ??????????????,???????????
    srand(systick_count); 

    lcd_fillRectangle(BG_COLOR, 0, 240, 0, 320); // ??

    // ?????,????????????
    while (1) {
        switch (current_state) {
            case STATE_START_MENU:
                handle_start_menu();
                break;
            case STATE_IN_GAME:
                handle_in_game();
                break;
            case STATE_GAME_OVER:
                handle_end_screen(0); // 0 ???
                break;
            case STATE_GAME_WIN:
                handle_end_screen(1); // 1 ???
                break;
        }
    }
}

// =================================================================================
// ???????? (State Handler Implementations)
// =================================================================================

void handle_start_menu(void) {
    char str_buf[20];
    int i;

    // ??????
    lcd_fillRectangle(BG_COLOR, 0, 240, 0, 320);
    
    // ?????? (?? CFONT.H ?????)
    // !!??!!: ????? CFONT.H ??,?????????? (0, 1, 2, 3, 4, 5)
    // ?? 0, 1, 2 ????? "???"
    lcd_showChinChar(20, 20, 0, TEXT_COLOR, BG_COLOR);
    lcd_showChinChar(20 + 16, 20, 1, TEXT_COLOR, BG_COLOR);
    lcd_showChinChar(20 + 32, 20, 2, TEXT_COLOR, BG_COLOR);
    draw_string(20 + 56, 20, "1155123456", TEXT_COLOR);

    // ?? 3, 4, 5 ?????? "???"
    lcd_showChinChar(20, 50, 3, TEXT_COLOR, BG_COLOR);
    lcd_showChinChar(20 + 16, 50, 4, TEXT_COLOR, BG_COLOR);
    lcd_showChinChar(20 + 32, 50, 5, TEXT_COLOR, BG_COLOR);
    draw_string(20 + 56, 50, "1155654321", TEXT_COLOR);

    // ?? Level ??
    for (i = 1; i <= 6; i++) {
        u16 color = (i == selected_level_menu) ? HIGHLIGHT_COLOR : TEXT_COLOR;
        if (i <= 5) {
            sprintf(str_buf, "Level %d", i);
        } else {
            sprintf(str_buf, "Infinity Mode");
        }
        draw_string(40, 100 + i * 20, str_buf, color);
    }

    // ????,??????
    while (current_state == STATE_START_MENU) {
        // ?? KEYx_Read() ??? IERG3810_KEY.c ??
        if (KEYUP_Read()) { // KEY_UP (PA0) is active high, ??????
            while(KEYUP_Read()); // ??????
            selected_level_menu--;
            if (selected_level_menu < 1) selected_level_menu = 6;
            break; // ?????????
        }
        if (!KEY1_Read()) { // KEY1 (PE3) is active low, ??????
            while(!KEY1_Read()); // ??????
            selected_level_menu++;
            if (selected_level_menu > 6) selected_level_menu = 1;
            break; // ?????????
        }
        if (!KEY2_Read()) { // KEY2 (PE2) is active low, ????
            while(!KEY2_Read()); // ??????
            current_level = selected_level_menu;
            game_init(current_level); // ?????
            current_state = STATE_IN_GAME; // ???????
            break;
        }
    }
}

void handle_in_game(void) {
    handle_game_input(); // ??????

    // ?? SysTick ??????????,???????
    if (systick_count - last_move_time >= game_speed_delay) {
        update_snake();
        check_collisions();
        last_move_time = systick_count;
    }
    
    // ?? Level 4, 5,?????
    if ((current_level == 4 || current_level == 5) && (systick_count - last_rock_move_time >= rock_move_delay)) {
        update_moving_rocks();
        last_rock_move_time = systick_count;
    }
}

void handle_end_screen(u8 is_win) {
    char str_buf[20];
    
    lcd_fillRectangle(BG_COLOR, 0, 240, 0, 320);

    if (is_win) {
        draw_string(80, 120, "YOU WIN!", c_GREEN);
    } else {
        draw_string(80, 120, "GAME OVER", c_RED);
    }
    
    sprintf(str_buf, "Score: %d", score);
    draw_string(80, 150, str_buf, TEXT_COLOR);

    if (score > high_score) {
        high_score = score;
    }

    Buzzer_On(); // ?? IERG3810_Buzzer.c
    delay(100000); // ?? IERG3810_USART.c
    Buzzer_Off();
    
    delay(3000000); // ?????????

    current_state = STATE_START_MENU; // ?????
}

// =================================================================================
// ???????? (Game Logic Implementations)
// =================================================================================

void game_init(int level) {
    lcd_fillRectangle(BG_COLOR, 0, 240, 0, 320);
    draw_game_board();
    
    score = 0;
    
    // ????
    snake.length = 3;
    snake.body[0] = (Point){GRID_SIZE / 2, GRID_SIZE / 2};
    snake.body[1] = (Point){GRID_SIZE / 2 - 1, GRID_SIZE / 2};
    snake.body[2] = (Point){GRID_SIZE / 2 - 2, GRID_SIZE / 2};
    snake.direction = (Point){1, 0}; // ??????

    // ??level??????
    num_rocks = 0;
    
    switch(level) {
        case 1: num_fruits_to_win = 5; game_speed_delay = 20; break;
        case 2: num_fruits_to_win = 8; game_speed_delay = 17; break;
        case 3: num_rocks = 3; num_fruits_to_win = 10; game_speed_delay = 14; break;
        case 4: num_rocks = 4; num_fruits_to_win = 12; game_speed_delay = 11; rock_move_delay = 150; break;
        case 5: num_rocks = 5; num_fruits_to_win = 15; game_speed_delay = 8; rock_move_delay = 100; break;
        case 6: // Infinity Mode
            num_fruits_to_win = 9999; // ????
            game_speed_delay = 20;
            break;
    }
    
    place_item_randomly(&fruit);
    
    for (int i = 0; i < num_rocks; i++) {
        place_item_randomly(&rocks[i]);
    }
    
    if (level == 1 || level == 6) {
        place_item_randomly(&portals[0]);
        place_item_randomly(&portals[1]);
    }

    draw_snake();
    draw_all_items();
    update_info_display();
    
    last_move_time = systick_count;
    last_rock_move_time = systick_count;
}

void place_item_randomly(Point* item) {
    int is_occupied;
    do {
        is_occupied = 0;
        item->x = rand() % GRID_SIZE;
        item->y = rand() % GRID_SIZE;

        // ?????????
        for (int i = 0; i < snake.length; i++) {
            if (item->x == snake.body[i].x && item->y == snake.body[i].y) {
                is_occupied = 1;
                break;
            }
        }
        if (is_occupied) continue;

        // ??????????? (??, ??, ???)
        if (item != &fruit && item->x == fruit.x && item->y == fruit.y) is_occupied = 1;
        for (int i = 0; i < num_rocks; i++) {
            if (item != &rocks[i] && item->x == rocks[i].x && item->y == rocks[i].y) {
                is_occupied = 1;
                break;
            }
        }
        if (current_level == 1 || current_level == 6) {
            for (int i = 0; i < 2; i++) {
                if (item != &portals[i] && item->x == portals[i].x && item->y == portals[i].y) {
                    is_occupied = 1;
                    break;
                }
            }
        }

    } while (is_occupied);
}

void handle_game_input(void) {
    // ???????????,???180???
    if (KEYUP_Read() && snake.direction.y == 0) { // ?
        snake.direction = (Point){0, -1};
    } else if (!KEY1_Read() && snake.direction.y == 0) { // ?
        snake.direction = (Point){0, 1};
    } else if (!KEY2_Read() && snake.direction.x == 0) { // ?
        snake.direction = (Point){-1, 0};
    } else if (!KEY0_Read() && snake.direction.x == 0) { // ?
        snake.direction = (Point){1, 0};
    }
    
    // ?????? KEY0 ??
    if (!KEY0_Read()) {
        u32 press_time = systick_count;
        // ?????,?????
        while(!KEY0_Read()) {
            if (systick_count - press_time > 100) { // ??1? (100 * 10ms)
                current_state = STATE_GAME_OVER;
                return;
            }
        }
    }
}

void update_snake(void) {
    Point old_tail = snake.body[snake.length - 1];
    draw_cell(old_tail.x, old_tail.y, BG_COLOR); // ?LCD???????

    // ?????,???????????????????
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }

    // ??????????
    snake.body[0].x += snake.direction.x;
    snake.body[0].y += snake.direction.y;
    
    // ?? Level 1 ? Infinity Mode ?????
    if (current_level == 1 || current_level == 6) {
        if (snake.body[0].x < 0) snake.body[0].x = GRID_SIZE - 1;
        if (snake.body[0].x >= GRID_SIZE) snake.body[0].x = 0;
        if (snake.body[0].y < 0) snake.body[0].y = GRID_SIZE - 1;
        if (snake.body[0].y >= GRID_SIZE) snake.body[0].y = 0;
    }
    
    draw_cell(snake.body[0].x, snake.body[0].y, SNAKE_COLOR); // ??????
}

void check_collisions(void) {
    Point head = snake.body[0];

    // 1. ?? (level 2-5)
    if (current_level >= 2 && current_level <= 5) {
        if (head.x < 0 || head.x >= GRID_SIZE || head.y < 0 || head.y >= GRID_SIZE) {
            current_state = STATE_GAME_OVER;
            return;
        }
    }

    // 2. ???????
    for (int i = 1; i < snake.length; i++) {
        if (head.x == snake.body[i].x && head.y == snake.body[i].y) {
            current_state = STATE_GAME_OVER;
            return;
        }
    }

    // 3. ???? (level 3-5)
    for (int i = 0; i < num_rocks; i++) {
        if (head.x == rocks[i].x && head.y == rocks[i].y) {
            current_state = STATE_GAME_OVER;
            return;
        }
    }

    // 4. ????
    if (head.x == fruit.x && head.y == fruit.y) {
        score += 10;
        if (snake.length < MAX_SNAKE_LENGTH) {
            snake.length++; // ???
        }
        
        // Infinity mode ???????
        if (current_level == 6 && game_speed_delay > 5) {
            game_speed_delay--;
        }

        if (current_level != 6 && (score / 10) >= num_fruits_to_win) {
            current_state = STATE_GAME_WIN;
            return;
        }
        
        place_item_randomly(&fruit);
        draw_cell(fruit.x, fruit.y, FRUIT_COLOR);
        update_info_display();
        Buzzer_On();
        delay(10000);
        Buzzer_Off();
    }
    
    // 5. ????? (level 1, 6)
    if (current_level == 1 || current_level == 6) {
        if (head.x == portals[0].x && head.y == portals[0].y) {
            snake.body[0] = portals[1]; // ???????
						draw_cell(portals[0].x, portals[0].y, PORTAL_COLOR_1); // ????
						Buzzer_On(); delay(10000); Buzzer_Off();
        } else if (head.x == portals[1].x && head.y == portals[1].y) {
            snake.body[0] = portals[0];
						draw_cell(portals[1].x, portals[1].y, PORTAL_COLOR_2); // ????
						Buzzer_On(); delay(10000); Buzzer_Off();
        }
    }
}

void update_moving_rocks(void) {
    for (int i = 0; i < num_rocks; i++) {
        Point old_pos = rocks[i];
        int move_dir = rand() % 4; // 0:?, 1:?, 2:?, 3:?
        Point new_pos = old_pos;
        
        if (move_dir == 0 && new_pos.y > 0) new_pos.y--;
        else if (move_dir == 1 && new_pos.y < GRID_SIZE - 1) new_pos.y++;
        else if (move_dir == 2 && new_pos.x > 0) new_pos.x--;
        else if (move_dir == 3 && new_pos.x < GRID_SIZE - 1) new_pos.x++;

        // ??????????
        int is_occupied = 0;
        for (int j = 0; j < snake.length; j++) {
            if (new_pos.x == snake.body[j].x && new_pos.y == snake.body[j].y) {
                is_occupied = 1;
                break;
            }
        }
        
        if (!is_occupied) {
            rocks[i] = new_pos;
            draw_cell(old_pos.x, old_pos.y, BG_COLOR);
            draw_cell(new_pos.x, new_pos.y, ROCK_COLOR);
        }
    }
}

// =================================================================================
// ?????? (Drawing Function Implementations)
// =================================================================================

void draw_cell(int x, int y, u16 color) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
    u16 px = BOARD_START_X + x * CELL_PIXEL_SIZE;
    u16 py = BOARD_START_Y + y * CELL_PIXEL_SIZE;
    // ????,??1?????????
    lcd_fillRectangle(color, px + 1, CELL_PIXEL_SIZE - 2, py + 1, CELL_PIXEL_SIZE - 2);
}

void draw_game_board(void) {
    // ????????????
    lcd_fillRectangle(BG_COLOR, BOARD_START_X, GRID_SIZE * CELL_PIXEL_SIZE, BOARD_START_Y, GRID_SIZE * CELL_PIXEL_SIZE);
    for (int i = 0; i <= GRID_SIZE; i++) {
        // ???
        lcd_fillRectangle(GRID_COLOR, BOARD_START_X + i * CELL_PIXEL_SIZE, 1, BOARD_START_Y, GRID_SIZE * CELL_PIXEL_SIZE);
        // ???
        lcd_fillRectangle(GRID_COLOR, BOARD_START_X, GRID_SIZE * CELL_PIXEL_SIZE, BOARD_START_Y + i * CELL_PIXEL_SIZE, 1);
    }
}

void draw_snake(void) {
    for (int i = 0; i < snake.length; i++) {
        draw_cell(snake.body[i].x, snake.body[i].y, SNAKE_COLOR);
    }
}

void draw_all_items(void) {
    draw_cell(fruit.x, fruit.y, FRUIT_COLOR);
    for (int i = 0; i < num_rocks; i++) {
        draw_cell(rocks[i].x, rocks[i].y, ROCK_COLOR);
    }
    if (current_level == 1 || current_level == 6) {
        draw_cell(portals[0].x, portals[0].y, PORTAL_COLOR_1);
        draw_cell(portals[1].x, portals[1].y, PORTAL_COLOR_2);
    }
}

void update_info_display(void) {
    char str_buf[25];
    
    // ??????????
    lcd_fillRectangle(BG_COLOR, 0, 240, 245, 75);
    
    sprintf(str_buf, "Score: %d", score);
    draw_string(10, 260, str_buf, TEXT_COLOR);

    sprintf(str_buf, "High Score: %d", high_score);
    draw_string(10, 280, str_buf, TEXT_COLOR);
    
    draw_string(10, 300, "Hold KEY0 to Quit", TEXT_COLOR);
}

// ????????????
void draw_string(u16 x, u16 y, char* str, u16 color) {
    int i = 0;
    while(str[i] != '\0') {
        lcd_showChar(x + i * 8, y, str[i], color, BG_COLOR);
        i++;
    }
}
