/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to create and move objects.
 */

#ifndef OBJECTS_H
#define OBJECTS_H

#include "system.h"

#define ITEMS 4

/* Struct to hold coordinate values */
typedef struct {
    int col;
    int row;
    int state;
} Pair;

/* Struct that holds a list of coordinates (a shape) */
typedef struct{
    Pair coords[4];
} Shape;

/* Functions to be used outisde of objects.c */
Shape* get_shape(void);

void drop(Shape* current_object);

void hide_shape(Pair coord_list[ITEMS]);

void show_shape(Pair coord_list[ITEMS]);

bool shift_down (Pair coord_list[ITEMS]);

void shift_left (Pair coord_list[ITEMS]);

void shift_right (Pair coord_list[ITEMS]);

#endif //OBJECTS_H
