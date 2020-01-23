//
//  ToF_processing.hpp
//  ToF Sensors Processing
//
//  Created by Shanti Stewart on 1/22/20.
//  Copyright © 2020 ToF Sensor Code. All rights reserved.
//



#ifndef ToF_processing_hpp
#define ToF_processing_hpp


// constants:
const int NUM_SENSORS = 7;
const int NUM_SAMPLES = 1;
const int MAX_OBJECTS = 4;
const float ANGLE_SEP = 15.0;
const int MAX_DISTANCE = 500;
const int OUT_OF_RANGE = 1000;

// struct to contain data about object(s):
struct object_data {
	int num_objects;
	int distance[MAX_OBJECTS][2];
	float angle[MAX_OBJECTS][2];
};


void init_object_data(object_data* objects);

float weight_avg(int data[], int size);

float calc_angle(int sensor_num);

object_data process_ToF(int data[NUM_SENSORS][NUM_SAMPLES]);


#endif /* ToF_processing_hpp */
