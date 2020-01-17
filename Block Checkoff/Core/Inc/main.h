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
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct __FILE {
	int dummy;
};


// struct pulled from openmv repository
typedef struct image {
    int w;
    int h;
    int bpp;
    union {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;


/*
 * for obj_t class variable:	0 = unidentified, 1 = robot, 2 = hub
 * 								3 = wall, 4 = charging pad
 *
 * size is a best guess based upon typical values for the area that the object
 * takes up in the image in cm^2
 */

typedef struct object {
	float hdng1;
	float hdng2;
	uint8_t size_approx;
	uint8_t distance;
	uint8_t obj_class;
	uint8_t obj_state;
} obj_t;

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
void Camera_Setup(uint8_t*);
bool send_UART(uint8_t*, uint8_t*);
int _write(int, char*, int);
void Camera_Snapshot(uint8_t*);
void Camera_Get_IMG_Length(uint8_t*);
void Camera_Get_IMG(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t);
void Camera_Set_Res(uint8_t*, uint8_t);
void get_TOF_Data(uint8_t*);
void get_Compass_Data(uint8_t*);
obj_t* process_IMG(image_t);
void find_Obj(image_t*, obj_t*);
void find_Headings(image_t*, obj_t*);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
