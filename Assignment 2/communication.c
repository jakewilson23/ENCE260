/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to receive and transmit data to another player
 */

#include <stdio.h>
#include <string.h>
#include "system.h"
#include "communication.h"
#include "led_output.h"
#include "pacer.h"
#include "objects.h"
#include "timer.h"
#include "ir_uart.h"
#include "navswitch.h"
#include "tinygl.h"

void find_player(void)
{
    tinygl_text(" Waiting...");
    while(true) {
        pacer_wait();
        tinygl_update();
        navswitch_update();
        if (ir_uart_read_ready_p()) {
            int data = ir_uart_getc();
            if (data == START) {
                ir_uart_putc(START);
                return;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc(START);
        }
    }
}


void display_outcome(bool win)
{
    // Array of length 10 in order to hold the entire scoreboard.
    char my_score_text[10] = {};
    char enemy_score_text[10] = {};
    static int my_score = 0;
    static int enemy_score = 0;


    if (win) {
        my_score++;
    } else {
        enemy_score++;
    }

    sprintf(my_score_text, "%d", my_score);
    sprintf(enemy_score_text, "-%d", enemy_score);
    strcat(my_score_text, enemy_score_text);
    tinygl_text(my_score_text);
    
    while(!navswitch_push_event_p(NAVSWITCH_PUSH))
    {
        pacer_wait ();
        tinygl_update();
        navswitch_update();
    }
}

char receive_data(void)
{
    if (ir_uart_read_ready_p ()) {
        int data = ir_uart_getc();
        if (data == WIN) {
            return WIN;
        } else if (data == LINE) {
            return LINE;
        }
    }
    return '-';
}

/* Return true if you have lost the game, else return false*/
bool check_loss(void)
{
    if (top_led_on()) {
        /* Send the value WIN if the player has lost */
        ir_uart_putc(WIN);
        return true;
    }
    return false;
}

/* If a full layer is lit up on, then remove the layer and shift all rows above it down */
void remove_completed (void)
{
    int total_removed = 0;
    for (int i = 6; i >= 0; i--) {
        bool completed = layer_filled(i);
        if (completed) {
            total_removed++;
            remove_layer(i);
        }
    }
    if (total_removed == 2) {
        ir_uart_putc(LINE);
    }
}