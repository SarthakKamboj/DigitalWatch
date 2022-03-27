/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "font_data.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

#define DISPLAY_COLS 128
#define DISPLAY_ROWS 64
#define DISPLAY_I2C_ADDR 0x3C

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char display_buffer[DISPLAY_COLS * DISPLAY_ROWS / 8];

void draw_character(int row, int horizontal_offset, uint8_t* start_of_data) {
	for (int col = 0; col < 8; col++) {
		uint8_t data = 0x00;
		for (int font_data_row = 0; font_data_row < 8; font_data_row++) {
			data |= ((start_of_data[font_data_row] >> (7 - col)) & 0x01) << font_data_row;
		}
		display_buffer[(row * DISPLAY_COLS) + col + horizontal_offset] = data;
	}
}

void draw_digit(int digit, int row, int horizontal_offset) {
	int font_number_data_idx = digit * 8;
	for (int col = 0; col < 8; col++) {
		draw_character(0, horizontal_offset, font_number_data + font_number_data_idx);
//		uint8_t data = 0x00;
//		for (int font_number_data_row = 0; font_number_data_row < 8; font_number_data_row++) {
//			data |= ((font_number_data[font_number_data_row + font_number_data_idx] >> (7 - col)) & 0x01) << font_number_data_row;
//		}
//		display_buffer[(row * DISPLAY_COLS) + col + horizontal_offset] = data;
	}
}

// TODO: add size capability
void draw_number(int row) {

	draw_digit(0, 0, 0);
	draw_digit(3, 0, 8);

	draw_digit(2, 0, 24);
	draw_digit(6, 0, 32);

//	for (int number = 0; number < 10; number++) {
//		draw_digit(number, 0, 8 * number);
//		int font_number_data_idx = number * 8;
//		for (int col = 0; col < 8; col++) {
//			uint8_t data = 0x00;
//			for (int font_number_data_row = 0; font_number_data_row < 8; font_number_data_row++) {
//				data |= ((font_number_data[font_number_data_row + font_number_data_idx] >> (7 - col)) & 0x01) << font_number_data_row;
//			}
//			display_buffer[(row * DISPLAY_COLS) + col + (8 * number)] = data;
//		}
//	}

}

void print_to_screen(unsigned char* text) {
	HAL_UART_Transmit(&huart2, text, strlen((char*)text), HAL_MAX_DELAY);
}

void send_data(uint8_t* data, uint16_t size) {
	uint8_t padded_data[100];
	padded_data[0] = 0x40;
	for (int i = 0; i < size; i++) {
		padded_data[i+1] = data[i];
	}
	HAL_I2C_Master_Transmit(&hi2c1, DISPLAY_I2C_ADDR << 1, padded_data, size + 1, HAL_MAX_DELAY);
}

void send_cmd_to_display(uint8_t* data) {
	uint8_t padded_data[2];
	padded_data[0] = 0x00;
	padded_data[1] = *data;
	HAL_I2C_Master_Transmit(&hi2c1, DISPLAY_I2C_ADDR << 1, padded_data, 2, HAL_MAX_DELAY);
}

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
}

uint8_t buffer[1];

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2, buffer, sizeof(buffer));

  char buffer[20];
  sprintf(buffer, "test print\r\n");
  print_to_screen((unsigned char*)buffer);


  HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, DISPLAY_I2C_ADDR << 1, 5, HAL_MAX_DELAY);
  if (ret != HAL_OK) {
	  unsigned char buffer[20];
	  sprintf((char*)buffer, "error occurred\r\n");
	  print_to_screen(buffer);
  } else {
	  unsigned char buffer[40];
	  sprintf((char*)buffer, "display module is ready\r\n");
	  print_to_screen(buffer);
  }

  init_display();
  clear_display();
  draw_number(0);
  render_display();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {

	clear_display();
	uint8_t row = buffer[0] - '0';
	if (row < 0 || row > 7) {
		uint8_t str_buffer[50];
		sprintf((char*)str_buffer, "row %c is invalid\r\n", buffer[0]);
		print_to_screen(str_buffer);
		HAL_UART_Receive_IT(&huart2, buffer, 1);
		return;
	}
	for (int col = 0; col < DISPLAY_COLS; col++) {
		for (int i = row * 8; i < (row + 1) * 8; i++) {
			display_buffer[((i * DISPLAY_COLS) + col) / 8] = 0xff;
		}
	}
	render_display();

	HAL_UART_Receive_IT(&huart2, buffer, 1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
