/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to rotate objects.
 */

#include "system.h"
#include "rotation.h"
#include "objects.h"
#include "led_output.h"

/* Checking rotated location to see if any are lit up, returns false if this is the case as therefore
 * the object shouldn't be rotated.
 */
static bool is_rotatable(Pair coord_list[ITEMS])
{
    for (int i = 0; i < ITEMS; i++) {
        /* Don't check locations that are not lit up (not part of the object) */
        if (coord_list[i].state != 0) {
            /* Checking varying locations based on the position in coord_list */
            switch (i) {
                case 0:
                    if (is_on(coord_list[i].col+1, coord_list[i].row)) {
                        return false;
                    }
                    break;
                case 1:
                    if (is_on(coord_list[i].col, coord_list[i].row-1)) {
                        return false;
                    }
                    break;
                case 2:
                    if (is_on(coord_list[i].col, coord_list[i].row+1)) {
                        return false;
                    }
                    break;
                case 3:
                    if (is_on(coord_list[i].col-1, coord_list[i].row)) {
                        return false;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return true;
}

/* To rotate the object simply rotate the state of the coordinates in the shape */
static void rotate(Shape* current_object)
{
    int temp = (*current_object).coords[0].state;

    (*current_object).coords[0].state = (*current_object).coords[2].state;

    (*current_object).coords[2].state = (*current_object).coords[3].state;

    (*current_object).coords[3].state = (*current_object).coords[1].state;

    (*current_object).coords[1].state = temp;
}

/* Hides the object and then checks if the object is able to be rotated, if it is rotate.
 * Show the shape at the end of the function whether or not it has been rotated.
 */
void rotate_shape(Shape* current_object)
{
    hide_shape((*current_object).coords);
    if (is_rotatable((*current_object).coords)) {
        rotate(current_object);
    }
    show_shape((*current_object).coords);
}