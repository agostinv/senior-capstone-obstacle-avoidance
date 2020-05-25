/*
 * VC0706_comm.h
 *
 *  Created on: May 24, 2020
 *      Author: victo
 */

#ifndef INC_VC0706_COMM_H_
#define INC_VC0706_COMM_H_

#include "stm32h7xx_hal.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

void Camera_Reset(UART_HandleTypeDef*, uint8_t*, uint8_t);
void Camera_Snapshot(UART_HandleTypeDef*, uint8_t*, uint8_t);
void Camera_Stop_Capture(UART_HandleTypeDef*, uint8_t*, uint8_t);
void Camera_Get_IMG_Length(UART_HandleTypeDef*, uint8_t*, uint8_t);
void Camera_Get_IMG(UART_HandleTypeDef*, uint8_t*, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t);
void Camera_Set_Res(UART_HandleTypeDef*, uint8_t*, uint8_t, uint8_t);
void Camera_Set_Compress(UART_HandleTypeDef*, uint8_t*, uint8_t, uint8_t);
void Camera_Set_Baud(UART_HandleTypeDef*, uint8_t*, uint8_t, uint32_t);
void Camera_Initialize(UART_HandleTypeDef*, uint8_t, uint8_t, uint32_t);
void UART_Init_Baud(UART_HandleTypeDef*, USART_TypeDef*, uint32_t);
bool send_UART(UART_HandleTypeDef*, uint8_t*, uint8_t*, uint8_t, uint16_t);
void Cam_Error_Handler();

#endif /* INC_VC0706_COMM_H_ */
