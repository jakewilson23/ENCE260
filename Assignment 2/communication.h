/* 
 * Block Battles on the UCFK4
 * Author 1: Jake Wilson
 * Author 2: Dominic McNulty
 * 
 * Module used to receive and transmit data to another player
 */

#ifndef SEND_LAYERS_H
#define SEND_LAYERS_H

#include "system.h"

#define WIN 'W'
#define LINE 'L'
#define START 'S'

/* Functions to be used outisde of communication.c */
void find_player(void);

void display_outcome(bool win);

char receive_data(void);

void remove_completed(void);

bool check_loss(void);

#endif //SEND_LAYERS_H