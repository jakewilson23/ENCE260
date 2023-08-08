/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 */

#include <stdlib.h>
#include <time.h>
#include "pio.h"
#include "pacer.h"
#include "led_output.h"
#include "objects.h"
#include "navswitch.h"
#include "communication.h"
#include "rotation.h"
#include "tinygl.h"
#include "ir_uart.h"
#include "../fonts/font5x7_1.h"


#define PACER_RATE 500
#define MESSAGE_RATE 20
#define SPEED_UP_RATE 5000 // Note Time is in milliseconds: game speeds up 1 hz every 5 seconds
#define START_SCREEN_RATE 2

/* Returns true if the object has been fully dropped */
static bool check_navswitch(Shape* current_object)
{
    /* Moving left if the navswitch is pushed to the left */
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        shift_left((*current_object).coords);
    } 
    
    /* Moving right if the navswitch is pushed to the right */
    else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        shift_right((*current_object).coords);
    } 
    
    /* Pressing down navswitch causes rotation */
    else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        rotate_shape(current_object);
    }

    /* Fully places the object at the lowest point it can be if the navswitch is pointed down */
    else if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        drop(current_object);
        return true;
    }
    return false;
}



/* Setting up the system, this should be the first process always. */
static void mainSetup(void)
{
    /* "True randomness" */
    srand(PINC ^ PINB);

    system_init();
    pacer_init(PACER_RATE);
    navswitch_init ();
    tinygl_init(PACER_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    ir_uart_init();


    /* Initialise LED matrix pins. */
    for (int i = 0; i < MAX_COL; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    for (int i = 0; i < MAX_ROW; i++) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }
}

int main (void)
{
    mainSetup();
    while(true) 
    {
        reset_screen();
        int SCREEN_RATE = START_SCREEN_RATE;
        Shape* current_object = NULL;
        int amount_received = 0;
        uint16_t counter = 0;
        bool placed = true;
        bool finished = false;
        find_player();

        while (!finished)
        {
            /* Increases the randomness of spawning objects due to the amount of calls being varied based on 
            * when an object is placed. Also updating the screen constantly to output the bitmap on the LED matrix.
            */
            update_screen ();
            pacer_wait ();
            rand();
            
            /* Keeps a tally of how many 2-liners the opposing player has gotten
            * Adds a junk layer per amount and resets the count every time you place your object.  
            */
            char data = receive_data();
            if (data == WIN) {
                free(current_object);
                display_outcome(true);
                finished = true;
            } else if (data == LINE) {
                amount_received++;
            }


            /* Creates a new shape if the previous shape has been placed */
            if (placed) {
                /* Free the previous object that was just placed
                * Note: will call free(NULL) however this is ok will simply free nothing.
                */
                free(current_object);

                placed = false;

                /* Add and remove layers after an object has been placed */
                remove_completed();
                add_layers(amount_received);
                amount_received = 0;

                /* Check to see if the player has lost after rows have been remove/added from the screen */
                if (check_loss()) {
                    free(current_object);
                    display_outcome(false);
                    finished = true;
                }

                current_object = get_shape();
            }

            /* Check navswitch for any movement */
            placed = check_navswitch(current_object);


            /* Moving down at a rate of SCREEN_RATE */
            if (counter % (PACER_RATE/SCREEN_RATE) == 0 && !placed) {
                placed = shift_down((*current_object).coords);
                
            }

            /* Integers auto check overflow */
            counter++;

            /* Increase SCREEN_RATE at a rate of SPEED_UP_RATE */
            if (counter % SPEED_UP_RATE == 0) {
                SCREEN_RATE++;
            }
        }
    }
}
