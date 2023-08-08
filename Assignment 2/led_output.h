/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to check values in the bitmap along with update them
 */

#ifndef LED_OUTPUT_H
#define LED_OUTPUT_H

#include "system.h"
#include "pio.h"
#include "objects.h"

#define MAX_ROW LEDMAT_ROWS_NUM
#define MAX_COL LEDMAT_COLS_NUM

/* Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO, 
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};

/* Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};



/* Functions to be used outisde of led_output.c */
void reset_screen(void);

void add_layers(int amount_received);

void remove_layer(int row);

bool layer_filled(int row);

bool top_led_on(void);

void set_data(Pair coordinate);

void update_screen (void);

bool is_on(int col, int row);

#endif //LED_OUTPUT_H
