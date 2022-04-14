/*
 * time.c
 *
 *  Created on: Apr 14, 2022
 *      Author: kambo
 */

#include "time.h"
#include "font_data.h"

int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int cumul_days[12] = {};

extern float centi_seconds_elapsed;
extern set_option cur_option_to_change;

display_time_t display_time;
display_time_t start_time;

void init_time() {
	int cum_day = 0;
	for (int month = 0; month < 12; month++) {
	  cum_day += days[month];
	  cumul_days[month] = cum_day;
	}

	start_time.month = 5;
	start_time.day = 29;
	start_time.hour = 23;
	start_time.minute = 59;
	start_time.second = 50;
}

void update_time() {
	int centi_seconds_elapsed_int = (int)centi_seconds_elapsed + get_start_centis();

	centi_seconds_elapsed_int /= 100;

	int cur_sec = centi_seconds_elapsed_int % 60;
	centi_seconds_elapsed_int /= 60;
	int cur_min = centi_seconds_elapsed_int % 60;
	centi_seconds_elapsed_int /= 60;
	int cur_hour = centi_seconds_elapsed_int % 24;
	centi_seconds_elapsed_int /= 24;
	int total_days = (centi_seconds_elapsed_int) % 365;
	int cur_month;

	if (total_days < days[0]) {
		cur_month = 0;
	} else {
		for (int month = 1; month < 12; month++) {
			if (total_days < cumul_days[month]) {
				cur_month = month;
				break;
			}
		}
	}

	int cur_day;
	if (cur_month == 0) {
		cur_day = total_days;
	} else {
		cur_day = total_days - cumul_days[cur_month - 1];
	}

	display_time.month = cur_month;
	display_time.day = cur_day;
	display_time.hour = cur_hour;
	display_time.minute = cur_min;
	display_time.second = cur_sec;
}

void draw_date() {

	int date_text_size = 2;
	const int num_date_characters = 5;

	const int date_text_width = FONT_WIDTH * date_text_size * num_date_characters;
	const int date_row = 0;

	int padding_left = (DISPLAY_COLS - date_text_width) / 2;

	set_padding(date_row, padding_left);
	set_padding(date_row + 1, padding_left);

	int month = display_time.month + 1;
	draw_digit(month / 10, date_row, date_text_size);
	draw_digit(month % 10, date_row, date_text_size);

	draw_character(font_char_data, date_row, date_text_size);

	int day = display_time.day + 1;
	draw_digit(day / 10, date_row, date_text_size);
	draw_digit(day % 10, date_row, date_text_size);

}

void draw_time() {
	int time_text_size = 2;

	const int time_row = 4;
	const int num_time_characters = 8;
	const int time_text_width = FONT_WIDTH * time_text_size * num_time_characters;

	int padding_left = (DISPLAY_COLS - time_text_width) / 2;
	set_padding(time_row, padding_left);
	set_padding(time_row + 1, padding_left);

	draw_digit(display_time.hour / 10, time_row, time_text_size);
	draw_digit(display_time.hour % 10, time_row, time_text_size);

	draw_character(font_char_data + 8, time_row, time_text_size);

	draw_digit(display_time.minute / 10, time_row, time_text_size);
	draw_digit(display_time.minute % 10, time_row, time_text_size);

	draw_character(font_char_data + 8, time_row, time_text_size);

	draw_digit(display_time.second / 10, time_row, time_text_size);
	draw_digit(display_time.second % 10, time_row, time_text_size);

}

int last_hide_toggle_time;
void display_in_change_mode() {

	static int hide = 0;

	if (centi_seconds_elapsed - last_hide_toggle_time > 50) {
		last_hide_toggle_time = centi_seconds_elapsed;
		hide = !hide;
	}

	int date_text_size = 2;
	const int num_date_characters = 5;

	const int date_text_width = FONT_WIDTH * date_text_size * num_date_characters;
	const int date_row = 0;

	int padding_left_date = (DISPLAY_COLS - date_text_width) / 2;

	int time_text_size = 2;

	const int time_row = 4;
	const int num_time_characters = 8;
	const int time_text_width = FONT_WIDTH * time_text_size * num_time_characters;

	int padding_left_time = (DISPLAY_COLS - time_text_width) / 2;

	set_padding(date_row, padding_left_date);
	set_padding(date_row + 1, padding_left_date);

	set_padding(time_row, padding_left_time);
	set_padding(time_row + 1, padding_left_time);

	int month = start_time.month + 1;
	if (cur_option_to_change == set_month) {
		if (hide == 0) {
			draw_digit(month / 10, date_row, date_text_size);
			draw_digit(month % 10, date_row, date_text_size);
		} else {
			place_but_dont_draw_digit(month / 10, date_row, date_text_size);
			place_but_dont_draw_digit(month % 10, date_row, date_text_size);
		}
	} else {
		draw_digit(month / 10, date_row, date_text_size);
		draw_digit(month % 10, date_row, date_text_size);
	}


	draw_character(font_char_data, date_row, date_text_size);

	int day = start_time.day + 1;
	if (cur_option_to_change == set_day) {
		if (hide == 0) {
			draw_digit(day / 10, date_row, date_text_size);
			draw_digit(day % 10, date_row, date_text_size);
		} else {
			place_but_dont_draw_digit(day / 10, date_row, date_text_size);
			place_but_dont_draw_digit(day % 10, date_row, date_text_size);
		}
	} else {
		draw_digit(day / 10, date_row, date_text_size);
		draw_digit(day % 10, date_row, date_text_size);
	}

	if (cur_option_to_change == set_hour) {
		if (hide == 0) {
			draw_digit(start_time.hour / 10, time_row, time_text_size);
			draw_digit(start_time.hour % 10, time_row, time_text_size);
		} else {
			place_but_dont_draw_digit(start_time.hour / 10, time_row, time_text_size);
			place_but_dont_draw_digit(start_time.hour % 10, time_row, time_text_size);
		}
	} else {
		draw_digit(start_time.hour / 10, time_row, time_text_size);
		draw_digit(start_time.hour % 10, time_row, time_text_size);
	}

	draw_character(font_char_data + 8, time_row, time_text_size);

	if (cur_option_to_change == set_minute) {
		if (hide == 0) {
			draw_digit(start_time.minute / 10, time_row, time_text_size);
			draw_digit(start_time.minute % 10, time_row, time_text_size);
		} else {
			place_but_dont_draw_digit(start_time.minute / 10, time_row, time_text_size);
			place_but_dont_draw_digit(start_time.minute % 10, time_row, time_text_size);
		}
	} else {
		draw_digit(start_time.minute / 10, time_row, time_text_size);
		draw_digit(start_time.minute % 10, time_row, time_text_size);
	}

	draw_character(font_char_data + 8, time_row, time_text_size);

	if (cur_option_to_change == set_sec) {
		if (hide == 0) {
			draw_digit(start_time.second / 10, time_row, time_text_size);
			draw_digit(start_time.second % 10, time_row, time_text_size);
		} else {
			place_but_dont_draw_digit(start_time.second / 10, time_row, time_text_size);
			place_but_dont_draw_digit(start_time.second % 10, time_row, time_text_size);
		}
	} else {
		draw_digit(start_time.second / 10, time_row, time_text_size);
		draw_digit(start_time.second % 10, time_row, time_text_size);
	}

//	HAL_Delay(500);
//	hide = !hide;

}

int get_start_centis() {
	int start_days_elapsed = 0;
	if (start_time.month > 0) {
		start_days_elapsed = cumul_days[start_time.month - 1];
	}
	start_days_elapsed += start_time.day;
	return ((start_days_elapsed * 24 * 60 * 60) + (start_time.hour * 60 * 60) + (start_time.minute * 60) + start_time.second) * 100;
}

void draw_digit(int digit, int row, int size) {
	int font_number_data_idx = digit * 8;
	draw_character(font_number_data + font_number_data_idx, row, size);
}

void place_but_dont_draw_digit(int digit, int row, int size) {

	int horizontal_offset = get_padding(row);

	if (horizontal_offset >= DISPLAY_COLS || row >= PAGE_ROWS) {
		return;
	}

	for (int row_offset = 0; row_offset < size; row_offset++) {
		int cur_row_padding = get_padding(row + row_offset);
		int new_padding = min(cur_row_padding + (size * FONT_WIDTH), DISPLAY_COLS);
		set_padding(row + row_offset, new_padding);
	}

}

void draw_character(const uint8_t* start_of_data, int row, int size) {

	int horizontal_offset = get_padding(row);

	if (horizontal_offset >= DISPLAY_COLS || row >= PAGE_ROWS) {
		return;
	}


	for (int col = 0; col < FONT_WIDTH; col++) {
		uint8_t data = 0x00;
		for (int font_data_row = 0; font_data_row < FONT_HEIGHT; font_data_row++) {
			data |= ((start_of_data[font_data_row] >> (7 - col)) & 0x01) << font_data_row;
		}

		uint8_t new_data[MAX_SIZE] = {};
		project_to_larger_num_size(data, size, new_data);
		for (int row_offset = 0; row_offset < size; row_offset++) {
			int cur_row = row + row_offset;
			if (cur_row >= DISPLAY_ROWS) break;
			for (int col_offset = 0; col_offset < size; col_offset++) {
				int cur_col = (col * size) + col_offset + horizontal_offset;

				if (cur_col >= DISPLAY_COLS) {
					break;
				}

				set_page_col(cur_row, cur_col, new_data[size - 1 - row_offset]);
			}
		}
	}

	for (int row_offset = 0; row_offset < size; row_offset++) {
		int cur_row_padding = get_padding(row + row_offset);
		int new_padding = min(cur_row_padding + (size * FONT_WIDTH), DISPLAY_COLS);
		set_padding(row + row_offset, new_padding);
	}
}
