/*
 * time.h
 *
 *  Created on: Apr 14, 2022
 *      Author: kambo
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include <stdio.h>
#include "graphics.h"
#include "util.h"

typedef struct display_time_t {
	int month, day, hour, minute, second;
} display_time_t;

typedef enum set_option {
	set_none, set_month, set_day, set_hour, set_minute, set_sec
} set_option;

void init_time();
void update_display_time();
void draw_date();
void draw_time();
void display_in_change_mode();
int get_start_centis();
void draw_digit(int digit, int row, int size);
void place_but_dont_draw_digit(int digit, int row, int size);
void draw_character(const uint8_t* start_of_data, int row, int size);
void move_to_next_set_option();
void change_val_of_cur_set_option(int delta);


#endif /* INC_TIME_H_ */
