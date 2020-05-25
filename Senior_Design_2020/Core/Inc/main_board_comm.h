/*
 * main_board_comm.h
 *
 *  Created on: May 11, 2020
 *      Author: victo
 */

#ifndef INC_MAIN_BOARD_COMM_H_
#define INC_MAIN_BOARD_COMM_H_

#include "main.h"

#define DATA_REQ		0x01

uint8_t package_data(struct object*, uint8_t, uint8_t**);


#endif /* INC_MAIN_BOARD_COMM_H_ */
