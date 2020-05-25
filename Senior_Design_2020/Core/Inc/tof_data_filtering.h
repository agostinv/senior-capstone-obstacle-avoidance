/*
 * tof_data_filtering.h
 *
 *  Created on: May 24, 2020
 *      Author: victo
 */

#ifndef INC_TOF_DATA_FILTERING_H_
#define INC_TOF_DATA_FILTERING_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_SENSORS			7
#define NUM_SAMPLES			5
#define MAX_OBJECTS			4
#define ANGLE_SEP			15.0
#define MAX_DISTANCE		500
#define OUT_OF_RANGE		1000

typedef struct object {
	uint8_t hdng1;
	uint8_t hdng2;
	uint8_t rng1;
	uint8_t rng2;
	uint8_t obj_class;
	uint8_t obj_state;
} obj_t;

float weight_avg(int[], int);
float calc_angle(int);
void process_ToF(obj_t*, uint8_t*, int[][NUM_SAMPLES]);

#endif /* INC_TOF_DATA_FILTERING_H_ */
