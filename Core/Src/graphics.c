/*
 * graphics.c
 *
 *  Created on: Apr 14, 2022
 *      Author: kambo
 */

#include "graphics.h"

int horizontal_offsets_by_page[DISPLAY_ROWS / PAGE_ROWS];
char display_buffer[DISPLAY_COLS * DISPLAY_ROWS / 8];

void init_display() {
	uint8_t DISPLAY_OFF = 0xAE;
	uint8_t DISPLAY_CLOCK_DIV = 0xD5;
	uint8_t DISPLAY_MULTIPLEX = 0xA8;
	uint8_t DISPLAY_OFFSET = 0xD3;
	uint8_t START_LINE = 0x40;
	uint8_t CHARGE_PUMP = 0x8D;
	uint8_t MEM_MODE = 0x20;
	uint8_t SEGREMAP = 0xA0 | 0x01;
	uint8_t COMSCANDEC = 0xC8;
	uint8_t SETCOMPINS = 0xDA;
	uint8_t CONTRAST = 0x81;
	uint8_t PRECHARGE = 0xD9;
	uint8_t SETVCOMDETECT = 0xDB;
	uint8_t DISPLAYALLON_RESUME = 0xA4;
	uint8_t NORMAL_DISPLAY = 0xA6;
	uint8_t DISPLAY_ON = 0xAF;
	uint8_t val;

	send_cmd_to_display(&DISPLAY_OFF);
	send_cmd_to_display(&DISPLAY_CLOCK_DIV);
	val = 0x80;
	send_cmd_to_display(&val);
	send_cmd_to_display(&DISPLAY_MULTIPLEX);
	val = 0x3F;
	send_cmd_to_display(&val);
	send_cmd_to_display(&DISPLAY_OFFSET);
	val = 0x00;
	send_cmd_to_display(&val);
	send_cmd_to_display(&START_LINE);
	send_cmd_to_display(&CHARGE_PUMP);
	val = 0x14;
	send_cmd_to_display(&val);
	send_cmd_to_display(&MEM_MODE);
	val = 0x00;
	send_cmd_to_display(&val);
	send_cmd_to_display(&SEGREMAP);
	send_cmd_to_display(&COMSCANDEC);
	send_cmd_to_display(&SETCOMPINS);
	val = 0x12;
	send_cmd_to_display(&val);
	send_cmd_to_display(&CONTRAST);
	val = 0xCF;
	send_cmd_to_display(&val);
	send_cmd_to_display(&PRECHARGE);
	val = 0xF1;
	send_cmd_to_display(&val);
	send_cmd_to_display(&SETVCOMDETECT);
	val = 0x40;
	send_cmd_to_display(&val);
	send_cmd_to_display(&DISPLAYALLON_RESUME);
	send_cmd_to_display(&NORMAL_DISPLAY);
	send_cmd_to_display(&DISPLAY_ON);
}

void render_display() {
	uint8_t COLUMN_ADDR = 0x21;
	uint8_t zero = 0;
	uint8_t RIGHT_MOST_COL = DISPLAY_COLS - 1;
	uint8_t PAGE_ADDR = 0x22;
	uint8_t val = 7;

	send_cmd_to_display(&COLUMN_ADDR);
	send_cmd_to_display(&zero);
	send_cmd_to_display(&RIGHT_MOST_COL);
	send_cmd_to_display(&PAGE_ADDR);
	send_cmd_to_display(&zero);
	send_cmd_to_display(&val);

	uint16_t i;
	for (i = 0; i < DISPLAY_ROWS * DISPLAY_COLS / 8; i++) {
		uint8_t temp_buffer[16];
		int size_of_temp_buffer = sizeof(temp_buffer) / sizeof(temp_buffer[0]);
		for (int x = 0; x < size_of_temp_buffer; x++) {
			temp_buffer[x] = display_buffer[i];
			i++;
		}
		send_data(temp_buffer, size_of_temp_buffer);
		i--;
	}
}

void clear_display() {
	memset(display_buffer, 0, sizeof(display_buffer) / sizeof(display_buffer[0]));
	memset(horizontal_offsets_by_page, 0, sizeof(horizontal_offsets_by_page) / sizeof(horizontal_offsets_by_page[0]));
}

void send_cmd_to_display(uint8_t* data) {
	uint8_t padded_data[2];
	padded_data[0] = 0x00;
	padded_data[1] = *data;
	HAL_I2C_Master_Transmit(&hi2c1, DISPLAY_I2C_ADDR << 1, padded_data, 2, HAL_MAX_DELAY);
}

void send_data(uint8_t* data, uint16_t size) {
	uint8_t padded_data[100];
	padded_data[0] = 0x40;
	for (int i = 0; i < size; i++) {
		padded_data[i+1] = data[i];
	}
	HAL_I2C_Master_Transmit(&hi2c1, DISPLAY_I2C_ADDR << 1, padded_data, size + 1, HAL_MAX_DELAY);
}


void project_to_larger_num_size(uint8_t original_data, int multiplier, uint8_t* new_data_ptr) {
	int bit_shift_on_cur_idx = 0;
	int cur_idx = 0;
	for (int bit_to_project = 0; bit_to_project < 8; bit_to_project++) {
		uint8_t bit = (original_data >> (7 - bit_to_project)) & 0x01;
		for (int m = 0; m < multiplier; m++) {
			uint8_t new_data_bit_selection_filter = 0x80 >> bit_shift_on_cur_idx;
			uint8_t bit_shifted = (bit == 1) ? new_data_bit_selection_filter : 0x00;
			uint8_t prev_data = new_data_ptr[cur_idx];
			new_data_ptr[cur_idx] = (prev_data & ~new_data_bit_selection_filter) | bit_shifted;
			if (bit_shift_on_cur_idx >= 7) {
				bit_shift_on_cur_idx = 0;
				cur_idx += 1;
			} else {
				bit_shift_on_cur_idx += 1;
			}
		}
	}
}

void set_padding(int row, int value) {
	horizontal_offsets_by_page[row] = value;
}

int get_padding(int row) {
	return horizontal_offsets_by_page[row];
}

void set_page_col(int row, int col, uint8_t data) {
	display_buffer[(row * DISPLAY_COLS) + col] = data;
}
