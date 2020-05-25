/*
 * main_board_comm.c
 *
 *  Created on: May 11, 2020
 *      Author: victo
 */

#include "main_board_comm.h"


// note: currently assumes no limit to objects detected, however this could be simply implemented if necessary
uint8_t package_data(struct object* obj_list, uint8_t obj_count, uint8_t buffer[][9]) {

	buffer[0][0] = DATA_REQ;
	buffer[0][1] = obj_count;
	for (int i = 0; i < 7; i++) {
		buffer[0][i+2] = 0;
	}

	for (int i = 1; i <= (obj_count/2 + obj_count%2); i++) {
		buffer[i][0] = DATA_REQ;
		buffer[i][1] = (obj_list[2*i-2].obj_class << 4) + (obj_list[2*i-2].rng1 >> 4);
		buffer[i][2] = ((obj_list[2*i-2].hdng1 & 60) << 2) + (obj_list[2*i-2].rng1 & 15);
		buffer[i][3] = ((obj_list[2*i-2].hdng1 & 3) << 6) + ((obj_list[2*i-2].rng2 & 252) >> 2);
		buffer[i][4] = ((obj_list[2*i-2].rng2 & 3) >> 6) + obj_list[2*i-2].hdng2;

		if (2*i <= obj_count) {
			buffer[i][5] = (obj_list[2*i-1].obj_class << 4) + (obj_list[2*i-1].rng1 >> 4);
			buffer[i][6] = ((obj_list[2*i-1].hdng1 & 60) << 2) + (obj_list[2*i-1].rng1 & 15);
			buffer[i][7] = ((obj_list[2*i-1].hdng1 & 3) << 6) + ((obj_list[2*i-1].rng2 & 252) >> 2);
			buffer[i][8] = ((obj_list[2*i-1].rng2 & 3) >> 6) + obj_list[2*i-1].hdng2;
		}

		else {
			buffer[i][5] = 0;
			buffer[i][6] = 0;
			buffer[i][7] = 0;
			buffer[i][8] = 0;
		}
	}
	return 0;
}
