/*
 * tof_data_filtering.c
 *
 *  Created on: May 24, 2020
 *      Author: victo
 */

#include "tof_data_filtering.h"

float weight_avg(int data[], int size) {
	// weights ratio:
	int ratio_denom = 3;
	// calculate weights vector:
	float weights[size];
	// first weight:
	weights[0] = 1.0;
	// other weights:
	for(int i=1; i<size; i++) {
		weights[i] = ((ratio_denom-1.0) * weights[i-1]) / ratio_denom;
	}

	// calculate weighted average (disregarding out of range values:
	int sample_num = 0;
	float total_val = 0;
	float total_weight = 0;
	for(int i=size-1; i>=0; i--) {
		// if distance is in range:
		if(data[i] <= MAX_DISTANCE) {
			total_val += weights[sample_num]*data[i];
			total_weight += weights[sample_num];
			sample_num++;
		}
	}
	float avg = total_val / total_weight;

	// if over half the values are out of range, mark average as out of range:
	if(size-sample_num > size/2) {
		avg = OUT_OF_RANGE;
	}

	return avg;
}

float calc_angle(int sensor_num) {
	// center sensor number:
	int center = NUM_SENSORS/2 + NUM_SENSORS%2;
	// angular heading of sensor sensor_num:
	float angle = ((sensor_num+1)-center) * ANGLE_SEP;

	return angle;
}

void process_ToF(obj_t* object_list, uint8_t * object_num, int data[][NUM_SAMPLES]) {

	// compute weighted averages (over samples) for all sensors:
	float avg_distance[NUM_SENSORS];
	for(int i=0; i<NUM_SENSORS; i++) {
		avg_distance[i] = weight_avg(data[i], NUM_SAMPLES);
	}

	// determine number of objects and their distance/angles:
	bool object_found = false;
	int object_count = 0;

	// for first NUM_SENSORS-1 sensors:
	for(int i=0; i<=NUM_SENSORS-2; i++) {
		// if searching for new object:
		if(object_found==false) {
			// if new object is detected:
			if(avg_distance[i] <= MAX_DISTANCE) {
				// note that object has been detected:
				object_found = true;
				// record left edge distance and sensor number:
				object_list[object_count].rng1 = ((uint8_t) avg_distance[i])*255/1000;
				object_list[object_count].hdng1 = ((uint8_t) (calc_angle(i) + 45))*255/100;
			}
		}
		// if searching for right edge ("end") of current object:
		else {
			// if right edge of object is detected:
			if(avg_distance[i] > MAX_DISTANCE) {
				// note that search is done with current object:
				object_found = false;
				// record right edge distance and sensor number:
				object_list[object_count].rng2 = ((uint8_t) avg_distance[i-1])*255/1000;
				object_list[object_count].hdng2 = ((uint8_t) (calc_angle(i-1) + 45))*255/100;
				// increment number of objects:
				object_count++;
			}
		}

	}


	// handle last sensor (NUM_SENSORS-1):
	// last sensor index:
	int i = NUM_SENSORS-1;
	// if searching for new object:
	if(object_found==false) {
		// if new object is detected:
		if(avg_distance[i] <= MAX_DISTANCE) {
			// note that object has been detected:
			object_found = true;
			// record left and right edge distances:
			object_list[object_count].rng1 = ((uint8_t) avg_distance[i])*255/1000;
			object_list[object_count].rng2 = ((uint8_t) avg_distance[i])*255/1000;
			// record left and right edge sensor numbers:
			object_list[object_count].hdng1 = ((uint8_t) (calc_angle(i) + 45))*255/100;
			object_list[object_count].hdng2 = ((uint8_t) (calc_angle(i) + 45))*255/100;
			// increment number of objects:
			object_count++;
		}
	}
	// if searching for right edge ("end") of current object:
	else {
		// if right edge of object is detected:
		if(avg_distance[i] > MAX_DISTANCE) {
			// record right edge distance and sensor number:
			object_list[object_count].rng2 = ((uint8_t) avg_distance[i-1])*255/1000;
			object_list[object_count].hdng2 = ((uint8_t) (calc_angle(i-1) + 45))*255/100;
			// increment number of objects:
			object_count++;
		}
		// if object extends past FOV:
		else {
			// record right edge distance and sensor number (even though object may extend past FOV):
			object_list[object_count].rng1 = ((uint8_t) avg_distance[i])*255/1000;
			object_list[object_count].hdng1 = ((uint8_t) (calc_angle(i) + 45))*255/100;
			// increment number of objects:
			object_count++;
		}
	}

	// record number of objects in struct:
	(*object_num) = object_count;
}

