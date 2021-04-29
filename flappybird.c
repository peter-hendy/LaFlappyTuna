//HMMMMMMMMMMMMMMMMMMMMMMMMMM

#include "os.h"

//Display is sideways
#define LCD_WIDTH  LCDHEIGHT
#define LCD_HEIGHT LCDWIDTH

#define BIRD_X (LCD_WIDTH/2)
#define PIPE_WIDTH 20
#define PIPE_GAP 80
#define BIRD_WIDTH 10
#define BOTTOM_PAD 10

volatile uint16_t score;
volatile uint8_t alive = 1;
volatile uint8_t started = 0;
volatile uint16_t pipeX = LCD_WIDTH;
volatile uint16_t pipeY = LCD_HEIGHT/2;
volatile uint16_t prevPipeY = LCD_HEIGHT/2;
volatile uint16_t pipeSpeed = 1;
volatile int birdY = LCD_HEIGHT/2;
volatile int birdVelocity = 0;
volatile rectangle prevBird = {0,0,0,0};

int check_switches(int);
int update_display(int);

void main(void) {
    os_init();

    os_add_task( check_switches,  10, 1);
    os_add_task( update_display,  10, 1);

    sei();
   

    do {
        
        display_string_xy("Press middle to start", (LCD_WIDTH/2) - 60, (LCD_HEIGHT/2));
        while(started == 0){}
        
        //OCR1A = 65535;

        score = 0;
        clear_screen();
        while(alive == 1){}

        //Show death Screen
        clear_screen();
        display_string_xy("Your score:", (LCD_WIDTH/2) - 50, (LCD_HEIGHT/2) - 30);
        char buffer[4];
        sprintf(buffer, "%03d", score);
        display_string_xy(buffer, (LCD_WIDTH/2) + 35, (LCD_HEIGHT/2) - 30);
        display_string_xy("Game Over", (LCD_WIDTH/2) - 20, LCD_HEIGHT/2);
        display_string_xy("Press middle to play again", (LCD_WIDTH/2) - 70, (LCD_HEIGHT/2)+30); 

        while(alive == 0){}
        clear_screen();

    } while(1);

}


int check_switches(int state) {

    if (get_switch_long(_BV(SWC)) || get_switch_short(_BV(SWC))) {
        if (started == 0 && alive == 0){
            alive = 1;
        }
        else if (started == 0){
            started = 1;

            pipeX = LCD_WIDTH;
            pipeY = LCD_HEIGHT/2;
            birdY = LCD_HEIGHT/2;
            prevPipeY = LCD_HEIGHT/2;
            pipeSpeed = 1;
            rectangle new = {0,0,0,0};
            prevBird = new;
            birdVelocity = 0;
            clear_screen();
        }
        //Increase bird velocity
        birdVelocity = birdVelocity + 11;
    }

    return state;
}

int update_display(int state) {
    //Check alive before running
    if (started == 1 && alive == 1){

        //Update bird velocity
        birdY = birdY - birdVelocity;
        birdVelocity = birdVelocity - 1;
        if (birdVelocity <= -2){
            birdVelocity = -2;
        }


        pipeX = pipeX - pipeSpeed;
        if (birdY <= 0){
            birdY = 0;
        }
        if (birdY + BIRD_WIDTH >= LCD_HEIGHT){
            birdY = LCD_HEIGHT - BIRD_WIDTH;
        }

        if (pipeX + PIPE_WIDTH <= 0){
            pipeX = LCD_WIDTH;
            pipeY = rand() % (LCD_HEIGHT-PIPE_GAP-BOTTOM_PAD);
            rectangle prevPipeBottom = {0, 2, 0, LCD_HEIGHT};
            fill_rectangle(prevPipeBottom, display.background);
        }


        //Collision check
        if ((BIRD_X <= pipeX + PIPE_WIDTH && BIRD_X >= pipeX) ||
            (BIRD_X + BIRD_WIDTH <= pipeX + PIPE_WIDTH && BIRD_X + BIRD_WIDTH >= pipeX)) {
            
            if (birdY < pipeY || birdY + BIRD_WIDTH > pipeY + PIPE_GAP){
                alive = 0;
                started = 0;
            }
        }


        //Write display.background over previous object
        rectangle prevPipeBottom = {pipeX+PIPE_WIDTH, pipeX+PIPE_WIDTH+pipeSpeed + PIPE_WIDTH, 0, prevPipeY};
        fill_rectangle(prevPipeBottom, display.background);

        rectangle prevPipeTop = {pipeX+PIPE_WIDTH, pipeX+PIPE_WIDTH+pipeSpeed + PIPE_WIDTH, prevPipeY + PIPE_GAP, LCD_HEIGHT};
        fill_rectangle(prevPipeTop, display.background);

        fill_rectangle(prevBird, display.background);
        

        //Draw objects in correct possition
        rectangle r1 = {pipeX, pipeX + PIPE_WIDTH, 0, pipeY};
        fill_rectangle(r1, GREEN);

        rectangle r2 = {pipeX, pipeX + PIPE_WIDTH, pipeY + PIPE_GAP, LCD_HEIGHT};
        fill_rectangle(r2, GREEN);

        rectangle r3 = {BIRD_X, BIRD_X + BIRD_WIDTH, birdY, birdY + BIRD_WIDTH};
        fill_rectangle(r3, BLUE);

        //Update score if necessary
        if (pipeX == BIRD_X-1){
            score++;
        }
        
        //Display score
        display_string_xy("Score:", 0, 0);
        char buffer[4];
        sprintf(buffer, "%03d", score);
        display_string_xy(buffer, 50, 0);

        //Store previous pipe and bird for removal
        prevPipeY = pipeY;
        prevBird = r3;
    }
    return state;
}