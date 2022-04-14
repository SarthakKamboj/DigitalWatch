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

display_time_t display;
display_time_t start_time;

void init_time() {
	int cum_day = 0;
	for (int month = 0; month < 12; month++) {
	  cum_day += days[month];
	  cumul_days[month] = cum_day;
	}
}

void update_time() {
	int centi_seconds_elapsed_int = (int)centi_seconds_elapsed + get_start_centis();

	centi_seconds_elapsed_int /= 100;

	int cur_sec = centi_seconds_elapsed_int % 60;
	int cur_min = centi_seconds_elapsed_int / 60;
	int cur_hour = cur_min / 60;
	int total_days = (cur_hour / 24) % 365;
	int cur_month;
	for (int month = 0; month < 12; month++) {
		if (total_days < cumul_days[month]) {
			cur_month = month;
			break;
		}
	}
	int cur_day;

	if (cur_month == 0) {
		cur_day = total_days;
	} else {
		cur_day = total_days - cumul_days[cur_month];
	}

	display.month = cur_month;
	display.day = cur_day;
	display.hour = cur_hour;
	display.minute = cur_min;
	display.second = cur_sec;
}

void draw_date() {

	int centi_seconds_elapsed_int = (int)centi_seconds_elapsed + get_start_centis();
	int days_elapsed = centi_seconds_elapsed_int / (100 * 60* 60 * 24);
	int days_in_cur_month = days[start_time.month];

	int date_text_size = 2;
	const int num_date_characters = 5;

	const int date_text_width = FONT_WIDTH * date_text_size * num_date_characters;
	const int date_row = 0;

	int padding_left = (DISPLAY_COLS - date_text_width) / 2;

//	horizontal_offsets_by_page[date_row] = (DISPLAY_COLS - date_text_width) / 2;
//	horizontal_offsets_by_page[date_row + 1] = (DISPLAY_COLS - date_text_width) / 2;

	set_padding(date_row, padding_left);
	set_padding(date_row + 1, padding_left);

	draw_digit(start_time.month / 10, date_row, date_text_size);
	draw_digit(start_time.month % 10, date_row, date_text_size);

	draw_character(font_char_data, date_row, date_text_size);

	draw_digit(start_time.day / 10, date_row, date_text_size);
	draw_digit(start_time.day % 10, date_row, date_text_size);

}

void draw_time() {
	int time_text_size = 2;

	const int time_row = 4;
	const int num_time_characters = 8;
	const int time_text_width = FONT_WIDTH * time_text_size * num_time_characters;

//	horizontal_offsets_by_page[time_row] = (DISPLAY_COLS - time_text_width) / 2;
//	horizontal_offsets_by_page[time_row + 1] = (DISPLAY_COLS - time_text_width) / 2;

	int padding_left = (DISPLAY_COLS - time_text_width) / 2;
	set_padding(time_row, padding_left);
	set_padding(time_row + 1, padding_left);

	int sec_tens = 0, sec_ones = 0, min_tens = 0, min_ones = 0, hour_tens = 0, hour_ones = 0;

	int centi_seconds_elapsed_int = (int)centi_seconds_elapsed + get_start_centis();
	centi_seconds_elapsed_int /= 100;

	int secs = centi_seconds_elapsed_int % 60;
	sec_ones = secs % 10;
	sec_tens = secs / 10;

	int mins = centi_seconds_elapsed_int / 60;
	min_ones = mins % 10;
	mins /= 10;
	min_tens = mins % 10;

	int hours = mins / 60;
	hour_ones = hours % 10;
	hours /= 10;
	hour_tens = hours % 10;

	draw_digit(hour_tens, time_row, time_text_size);
	draw_digit(hour_ones, time_row, time_text_size);

	draw_character(font_char_data + 8, time_row, time_text_size);

	draw_digit(min_tens, time_row, time_text_size);
	draw_digit(min_ones, time_row, time_text_size);

	draw_character(font_char_data + 8, time_row, time_text_size);

	draw_digit(sec_tens, time_row, time_text_size);
	draw_digit(sec_ones, time_row, time_text_size);

}

void display_in_change_mode() {

}

int get_start_centis() {
	int start_days_elapsed = 0;
	if (start_time.month > 0) {
		start_days_elapsed = cumul_days[start_time.month - 1];
	}
	start_days_elapsed += start_time.day;
	return ((start_time.day * 24 * 60 * 60) + (start_time.hour * 60 * 60) + (start_time.minute * 60) + start_time.second) * 100;
}

void draw_digit(int digit, int row, int size) {
	int font_number_data_idx = digit * 8;
	draw_character(font_number_data + font_number_data_idx, row, size);
}

void draw_character(const uint8_t* start_of_data, int row, int size) {

//	int horizontal_offset = horizontal_offsets_by_page[row];
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

//				display_buffer[(cur_row * DISPLAY_COLS) + cur_col] = new_data[size - 1 - row_offset];
				set_page_col(cur_row, cur_col, new_data[size - 1 - row_offset]);
			}
		}
	}

	for (int row_offset = 0; row_offset < size; row_offset++) {
//		horizontal_offsets_by_page[row + row_offset] = min(horizontal_offsets_by_page[row + row_offset] + (size * FONT_WIDTH), DISPLAY_COLS);
		int cur_row_padding = get_padding(row + row_offset);
		int new_padding = min(cur_row_padding + (size * FONT_WIDTH), DISPLAY_COLS);
		set_padding(row + row_offset, new_padding);
	}
}
