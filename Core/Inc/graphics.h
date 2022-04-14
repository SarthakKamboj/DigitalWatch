/*
 * graphics.h
 *
 *  Created on: Apr 14, 2022
 *      Author: kambo
 */

#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include <stdio.h>
#include "font_data.h"
#include "stm32f4xx_hal.h"

#define DISPLAY_COLS 128
#define DISPLAY_ROWS 64
#define DISPLAY_I2C_ADDR 0x3C
#define PAGE_ROWS 8
#define MAX_SIZE 5

extern I2C_HandleTypeDef hi2c1;

void init_display();
void render_display();
void clear_display();
void send_cmd_to_display(uint8_t* data);
void send_data(uint8_t* data, uint16_t size);
void set_padding(int row, int value);
int get_padding(int row);
void set_page_col(int row, int col, uint8_t data);

// original data: 01010110
// new data:	  000111000111000111111000
void project_to_larger_num_size(uint8_t original_data, int multiplier, uint8_t* new_data_ptr);

#endif /* INC_GRAPHICS_H_ */
