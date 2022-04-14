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

extern float centi_seconds_elapsed;

void init_time();
void update_time();
void draw_date();
void draw_time();
void display_in_change_mode();
int get_start_centis();
void draw_digit(int digit, int row, int size);
void draw_character(const uint8_t* start_of_data, int row, int size);


#endif /* INC_TIME_H_ */
