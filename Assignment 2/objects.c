/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to create and move objects.
 */

#include <stdlib.h>
#include "objects.h"
#include "led_output.h"
#include "communication.h"
#include "system.h"


/* Setting up the memory for a square and giving the coordinates of above the screen */
static Shape* spawn_square (void)
{
    /* Allocate the memory for shape */
    Shape* new_square = calloc(1, sizeof(Shape)); 
    
    /* Setup the square shape:
     *  00
     *  00
     */
    
    /* Top Left */
    (*new_square).coords[0].col = MAX_COL / 2 - 1;
    (*new_square).coords[0].row = MAX_ROW+1;
    (*new_square).coords[0].state = 1;

    /* Top Right */
    (*new_square).coords[1].col = MAX_COL / 2;
    (*new_square).coords[1].row = MAX_ROW+1;
    (*new_square).coords[1].state = 1;

    /* Bottom Left */
    (*new_square).coords[2].col = MAX_COL / 2 - 1;
    (*new_square).coords[2].row = MAX_ROW;
    (*new_square).coords[2].state = 1;

    /* Bottom Right */
    (*new_square).coords[3].col = MAX_COL / 2;
    (*new_square).coords[3].row = MAX_ROW;
    (*new_square).coords[3].state = 1;

    return new_square;
}

static Shape* spawn_triangle (void)
{
    /* Allocate the memory for shape */
    Shape* new_triangle = calloc(1, sizeof(Shape)); 
    
    /* Setup the triangle shape:
     *  0X
     *  00
     */
    
    /* Top Left */
    (*new_triangle).coords[0].col = MAX_COL / 2 - 1;
    (*new_triangle).coords[0].row = MAX_ROW+1;
    (*new_triangle).coords[0].state = 1;

    /* Top Right */
    (*new_triangle).coords[1].col = MAX_COL / 2;
    (*new_triangle).coords[1].row = MAX_ROW+1;
    (*new_triangle).coords[1].state = 0;

    /* Bottom Left */
    (*new_triangle).coords[2].col = MAX_COL / 2 - 1;
    (*new_triangle).coords[2].row = MAX_ROW;
    (*new_triangle).coords[2].state = 1;

    /* Bottom Right */
    (*new_triangle).coords[3].col = MAX_COL / 2;
    (*new_triangle).coords[3].row = MAX_ROW;
    (*new_triangle).coords[3].state = 1;

    return new_triangle;
}

static Shape* spawn_line (void)
{
    /* Allocate the memory for shape */
    Shape* new_line = calloc(1, sizeof(Shape)); 
    
    /* Setup the line shape:
     *  X0
     *  X0
     */

    /* Top Left */
    (*new_line).coords[0].col = MAX_COL / 2 - 1;
    (*new_line).coords[0].row = MAX_ROW+1;
    (*new_line).coords[0].state = 0;

    /* Top Right */
    (*new_line).coords[1].col = MAX_COL / 2;
    (*new_line).coords[1].row = MAX_ROW+1;
    (*new_line).coords[1].state = 1;

    /* Bottom Left */
    (*new_line).coords[2].col = MAX_COL / 2 - 1;
    (*new_line).coords[2].row = MAX_ROW;
    (*new_line).coords[2].state = 0;

    /* Bottom Right */
    (*new_line).coords[3].col = MAX_COL / 2;
    (*new_line).coords[3].row = MAX_ROW;
    (*new_line).coords[3].state = 1;

    return new_line;
}

#define PROBABILITY_SQUARE 10
#define PROBABILITY_TRIANGLE 45
#define PROBABILITY_LINE 45

Shape* get_shape()
{
    /* Creating a random integer using modular aritmetic 
     * This is used to pick a random shape and assign it
     * to the current falling object
     */

    /* Due to the width of the screen the probabilities of items are varied.
     *
     * Square : 10%
     * Triangle : 45%
     * Line : 45%
     */
    Shape* new_shape = NULL;
    int shape_type = rand() % (PROBABILITY_SQUARE + PROBABILITY_TRIANGLE + PROBABILITY_LINE);
    if (0 <= shape_type && shape_type <= PROBABILITY_SQUARE) {
        new_shape = spawn_square();
    } else if (shape_type <= PROBABILITY_SQUARE + PROBABILITY_TRIANGLE) {
        new_shape = spawn_triangle();
    } else if (shape_type <= PROBABILITY_SQUARE + PROBABILITY_TRIANGLE + PROBABILITY_LINE) {
        new_shape = spawn_line();
    }
    return new_shape;
}



void drop(Shape* current_object)
{
    while(!shift_down((*current_object).coords)) {
        ;
    }
}


/* Changes the bitmap values at the positions in coord_list to 0 without changing their states */
void hide_shape(Pair coord_list[ITEMS])
{
    for (int i = 0; i < ITEMS; i++) {
        if (coord_list[i].state != 0) {
            Pair temp = coord_list[i];
            temp.state = 0;
            set_data(temp);
        }
    }
}


/* Changes the bitmap values at the positions in coord_list */
void show_shape(Pair coord_list[ITEMS])
{
    for (int i = 0; i < ITEMS; i++) {
        if (coord_list[i].state != 0) {
            set_data(coord_list[i]);
        }
    }

}

/* Returns True if any LEDs are lit up below, or shape is at the bottom */
static bool check_below(Pair coord_list[ITEMS]) 
{
    for (int i = 2; i < ITEMS; i++) {
        if (coord_list[i].state == 0) {
            if (coord_list[i-2].row == 0) {
                return true;
            } else {
                if (coord_list[i-2].state != 0 && is_on(coord_list[i-2].col, coord_list[i-2].row-1)) {
                        return true;
                }
            }
        } else {
            if (coord_list[i].row == 0) {
                return true;
            } else if (is_on(coord_list[i].col, coord_list[i].row-1)) {
                return true;
            }
        }
    }
    return false;
}

/* Returns True if any LEDs are lit up to the left, or shape is at the left wall */
static bool check_left(Pair coord_list[ITEMS])
{
    for (int i = 0; i < ITEMS; i += 2) {
        if (coord_list[i].state == 0) {
            if (coord_list[i+1].col == 0) {
                return true;
            } else if (is_on(coord_list[i+1].col-1, coord_list[i+1].row)) {
                return true;
            }
        } else {
            if (coord_list[i].col == 0) {
                return true;
            } else if (is_on(coord_list[i].col-1, coord_list[i].row)) {
                return true;
            }
        }
    }
    return false;
}

/* Returns True if any LEDs are lit up to the right, or shape is at the right wall */
static bool check_right(Pair coord_list[ITEMS])
{
    for (int i = 1; i < ITEMS; i += 2) {
        if (coord_list[i].state == 0) {
            if (coord_list[i-1].col == MAX_COL-1) {
                return true;
            } else if (is_on(coord_list[i-1].col+1, coord_list[i-1].row)) {
                return true;
            }
        } else {
            if (coord_list[i].col == MAX_COL-1) {
                return true;
            } else if (is_on(coord_list[i].col+1, coord_list[i].row)) {
                return true;
            }
        }
    }
    return false;
} 


/* If check_below returns true, object is placed so return true, else shift the object down and return false */
bool shift_down (Pair coord_list[ITEMS])
{
    
    if (check_below(coord_list)) {
        return true;
    }
    hide_shape(coord_list);
    for (int i = 0; i < ITEMS; i++) {
        coord_list[i].row--;
    }
    show_shape(coord_list);
    return false;
}

/* If check_left returns true do not move object, else shift the object to the left */
void shift_left (Pair coord_list[ITEMS])
{
    if (check_left(coord_list)) {
        return;
    }
    hide_shape(coord_list);
    for (int i = 0; i < ITEMS; i++) {
        coord_list[i].col--;
    }
    show_shape(coord_list);
}


/* If check_right returns true do not move object, else shift the object to the right */
void shift_right (Pair coord_list[ITEMS])
{
    if (check_right(coord_list)) {
        return;
    }
    hide_shape(coord_list);
    for (int i = 0; i < ITEMS; i++) {
        coord_list[i].col++;
    }
    show_shape(coord_list);
}