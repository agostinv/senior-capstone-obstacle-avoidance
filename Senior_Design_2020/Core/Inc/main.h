/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_board_comm.h"
#include "tof_data_filtering.h"
#include "VC0706_comm.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */


typedef struct buffer_data {
	uint16_t jpeg_size;
	uint8_t jpeg[30000]; // note: contains ACK so we start image at +5
	uint8_t rgb[57600];  // exactly the size of the expanded RGB565 array
	struct buffer_data* next;

} buffer_link;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
int _write(int, char*, int);
void get_TOF_Data();

// completely dummy functions, meant to be revised and filled as needed for image processing
obj_t* process_IMG(image_t);
void find_Obj(image_t*, obj_t*);
void find_Headings(image_t*, obj_t*);

// attempts at rewriting HAL drivers to be a bit faster
HAL_StatusTypeDef UART_Fast_Transmit(UART_HandleTypeDef*, uint8_t*, uint16_t, uint32_t);
HAL_StatusTypeDef UART_Fast_Receive(UART_HandleTypeDef*, uint8_t*, uint16_t, uint32_t);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SH7_Pin GPIO_PIN_6
#define SH7_GPIO_Port GPIOC
#define SH6_Pin GPIO_PIN_7
#define SH6_GPIO_Port GPIOC
#define SH5_Pin GPIO_PIN_8
#define SH5_GPIO_Port GPIOC
#define SH4_Pin GPIO_PIN_9
#define SH4_GPIO_Port GPIOA
#define SH3_Pin GPIO_PIN_10
#define SH3_GPIO_Port GPIOA
#define SH2_Pin GPIO_PIN_11
#define SH2_GPIO_Port GPIOA
#define SH1_Pin GPIO_PIN_12
#define SH1_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define TOF_ADD1			0x19
#define TOF_ADD2			0x1A
#define TOF_ADD3			0x1B
#define TOF_ADD4			0x1C
#define TOF_ADD5			0x1D
#define TOF_ADD6			0x1E
#define TOF_ADD7			0x1F
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
