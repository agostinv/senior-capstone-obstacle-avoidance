//
//  ToF_processing.cpp
//  ToF Sensors Processing
//
//  Created by Shanti Stewart on 1/22/20.
//  Copyright © 2020 ToF Sensor Code. All rights reserved.
//



#include "ToF_processing.hpp"
#include <iostream>
#include <cmath>

using namespace std;


void init_object_data(object_data* objects) {
	// initialize num_objects to 0;
	objects->num_objects = 0;
	// initialize all possible object distances and angular headings to 0:
	for(int i=0; i<MAX_OBJECTS; i++) {
		for(int j=0; j<2; j++) {
			objects->distance[i][j] = 0;
			objects->angle[i][j] = 0;
		}
	}
}

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
	if(size-sample_num > floor(size/2)) {
		avg = OUT_OF_RANGE;
	}
	
	return avg;
}

float calc_angle(int sensor_num) {
	// center sensor number:
	int center = ceil((float) NUM_SENSORS / 2);
	// angular heading of sensor sensor_num:
	float angle = ((sensor_num+1)-center) * ANGLE_SEP;
	
	return angle;
}

object_data process_ToF(int data[NUM_SENSORS][NUM_SAMPLES]) {
	// declare object_data struct:
	object_data ToF;
	init_object_data(&ToF);
	
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
				ToF.distance[object_count][0] = avg_distance[i];
				ToF.angle[object_count][0] = calc_angle(i);
			}
		}
		// if searching for right edge ("end") of current object:
		else {
			// if right edge of object is detected:
			if(avg_distance[i] > MAX_DISTANCE) {
				// note that search is done with current object:
				object_found = false;
				// record right edge distance and sensor number:
				ToF.distance[object_count][1] = avg_distance[i-1];
				ToF.angle[object_count][1] = calc_angle(i-1);
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
			ToF.distance[object_count][0] = avg_distance[i];
			ToF.distance[object_count][1] = avg_distance[i];
			// record left and right edge sensor numbers:
			ToF.angle[object_count][0] = calc_angle(i);
			ToF.angle[object_count][1] = calc_angle(i);
			// increment number of objects:
			object_count++;
		}
	}
	// if searching for right edge ("end") of current object:
	else {
		// if right edge of object is detected:
		if(avg_distance[i] > MAX_DISTANCE) {
			// record right edge distance and sensor number:
			ToF.distance[object_count][1] = avg_distance[i-1];
			ToF.angle[object_count][1] = calc_angle(i-1);
			// increment number of objects:
			object_count++;
		}
		// if object extends past FOV:
		else {
			// cout << avg_distance[i] << endl;
			// record right edge distance and sensor number (even though object may extend past FOV):
			ToF.distance[object_count][1] = avg_distance[i];
			ToF.angle[object_count][1] = calc_angle(i);
			// increment number of objects:
			object_count++;
		}
	}
	
	// record number of objects in struct:
	ToF.num_objects = object_count;
	
	
	return ToF;
}
