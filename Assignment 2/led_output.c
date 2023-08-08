/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to check values in the bitmap along with update them
 */

#include <stdlib.h>
#include "led_output.h"
#include "objects.h"
#include "navswitch.h"


/* Creates the bitmap for where to display lights on LED screen.
 * This setup is made so that it will fit with the tetris game and hence ordered in that way.
 */
static uint8_t bitmap[7][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

/* Sets all LEDs in the bitmap to 0 */
void reset_screen(void)
{
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            bitmap[row][col] = 0;
        }
    }
}

/* Adds however many layers have been received to the bottom of the screen */
void add_layers(int amount_received)
{
    for (int i = 0; i < amount_received; i++) {
        /* Loops through all the rows shifting them upwards by 1 */
        for (int row = MAX_ROW-1; row >= 0; row--) {
            /* If we are at the bottom row, add a junk row which is fully lit up excluding 1*/
            if (row == 0) {
                int non_removed_col = rand() % MAX_COL;
                for (int col = 0; col < MAX_COL; col++) {
                    if (col != non_removed_col) {
                        bitmap[0][col] = 1;
                    } else {
                        bitmap[0][col] = 0;
                    }
                }
            } else {
                for (int col = 0; col < MAX_COL; col++) {
                    bitmap[row][col] = bitmap[row-1][col];
                }
            }
        }
    }
}

/* Removes the layer */
void remove_layer(int removed_row)
{
    /* Removes the layer that was sent to it and push all rows above it down */
    for (int col = 0; col < MAX_COL; col++) {
        Pair remove_point = {col, removed_row, 0};
        set_data(remove_point);
    }
    /* Shift down all rows from the removed point to 1 row lower */
    for (int row = removed_row; row < MAX_ROW; row++) {
        /* When shifting down highest row simply set all the LEDs to off */
        if (row == MAX_ROW-1) {
            for (int col = 0; col < MAX_COL; col++) {
                bitmap[row][col] = 0;
            }
        } else {
            for (int col = 0; col < MAX_COL; col++) {
                bitmap[row][col] = bitmap[row+1][col];
            }
        }
    }
}

/* Returns true if any LEDs on the top row are on
 * Should only be called after completed rows have been removed
 * and a new object is not yet spawned in.
 */
bool top_led_on(void)
{
    for (int col = 0; col < MAX_COL; col++) {
        if (is_on(col, MAX_ROW-1)) {
            return true;
        }
    }
    return false;
}

/* Returns true if an entire row of the bitmap is on (value of 1)*/
bool layer_filled(int row)
{
    /* Loops through all columns within the row, if any of them are off then return false since row is not complete, else true*/
    for (int col = 0; col < MAX_COL; col++) {
        if (!bitmap[row][col]) {
            return false;
        }
    }
    return true;
}

/* Boolean functions to check whether or not a specific coordinate is lit up */
bool is_on(int col, int row)
{
    /* First to checks is to account for indexing error.
     * The object should be able to move above the screen however not got out further then the columnns
    */
    if (row >= MAX_ROW) {
        return false;
    } else if (col < 0 || col > MAX_COL-1) {
        return true;
    }
    /* 4-col due to the setup of our matrix allowing for easier indexing when programming */
    if (bitmap[row][4-col] == 1) {
        return true;
    }
    return false;
}

/* Functions for turning on and off LEDs.
 * Note: 4-col is used to make it easier for the programmer to write coordinates with respect to the bitmap.
 */
void set_data(Pair coordinate)
{
    if (0 <= coordinate.row && coordinate.row <= MAX_ROW-1 && 0 <= coordinate.col && coordinate.col <= MAX_COL-1) {
        bitmap[coordinate.row][4-coordinate.col] = coordinate.state;
    }
}

/* Loops through all the rows turning the LEDs on if there is a 1 in the bitmap at that location */
static void display_row (uint8_t current_row)
{
    static uint8_t prev_row = 0;

    pio_output_high(rows[prev_row]);

    /* Update navswitch while all the rows are off, removes ghosting caused by navswitch_update() */
    navswitch_update ();

    for (int col = 0; col < MAX_COL; col++) {
        if (bitmap[current_row][col] == 1) {
            pio_output_low(cols[col]);
        } else {
            pio_output_high(cols[col]);
        }
    }
    pio_output_low(rows[current_row]);
    prev_row = current_row;
}

/* Loop through displaying each row using display_row() */
void update_screen (void)
{
    static uint8_t current_row = 0;

    display_row (current_row);
        
    current_row++;
    if (current_row > (LEDMAT_ROWS_NUM - 1)) {
        current_row = 0;
    }
}