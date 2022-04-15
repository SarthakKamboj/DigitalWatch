/*
 * util.c
 *
 *  Created on: Apr 14, 2022
 *      Author: kambo
 */

#include "util.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;
void print_to_screen(unsigned char* text) {
	HAL_UART_Transmit(&huart2, text, strlen((char*)text), HAL_MAX_DELAY);
}

int min(int a, int b) {
	return (a < b) ? a : b;
}
