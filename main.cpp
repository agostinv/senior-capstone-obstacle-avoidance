//
//  main.cpp
//  ToF Sensors Processing
//
//  Created by Shanti Stewart on 1/19/20.
//  Copyright © 2020 ToF Sensor Code. All rights reserved.
//



#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "ToF_processing.hpp"
#include <cmath>

using namespace std;


void display_test_data(int test_data[NUM_SENSORS][NUM_SAMPLES]) {
	cout << "Test data: { ";
	for(int i=0; i<=NUM_SENSORS-2; i++) {
		cout << "{";
		for(int j=0; j<=NUM_SAMPLES-2; j++) {
			cout << test_data[i][j] << ", ";
		}
		cout << test_data[i][NUM_SAMPLES-1] << "}, ";
	}
	cout << "{";
	for(int j=0; j<=NUM_SAMPLES-2; j++) {
		cout << test_data[NUM_SENSORS-1][j] << ", ";
	}
	cout << test_data[NUM_SENSORS-1][NUM_SAMPLES-1] << "} ";
	
	cout << "}" << endl;
}

void display_object_data(object_data objects) {
	// print number of objects:
	cout << "Number of objects: " << objects.num_objects << endl;
	
	// print object distances:
	cout << "Object distances: { ";
	for(int i=0; i<=objects.num_objects-2; i++) {
		// print left edge distance:
		cout << "[" << objects.distance[i][0] << ", ";
		// print right edge distance:
		cout << objects.distance[i][1] << "], ";
	}
	// print left edge distance:
	cout << "[" << objects.distance[objects.num_objects-1][0] << ", ";
	// print right edge distance:
	cout << objects.distance[objects.num_objects-1][1] << "] }" << endl;
	
	// print object angular headings:
	cout << "Object angular headings: { ";
	for(int i=0; i<=objects.num_objects-2; i++) {
		// print left edge angular heading:
		cout << "[" << objects.angle[i][0] << ", ";
		// print right edge angular heading:
		cout << objects.angle[i][1] << "], ";
	}
	// print left edge angular heading:
	cout << "[" << objects.angle[objects.num_objects-1][0] << ", ";
	// print right edge angular heading:
	cout << objects.angle[objects.num_objects-1][1] << "] }" << endl;
}

void reset_test_data(int test_data[NUM_SENSORS][NUM_SAMPLES], bool in_range) {
	// set test_data[][] to values that are all in range: { {1,2,...,NUM_SAMPLES}, {NUM_SAMPLES+1,...,2*NUM_SAMPLES}, {2*NUM_SAMPLES+1,...,3*NUM_SAMPLES}, ..., {(NUM_SENSORS-1)*NUM_SAMPLES + 1,...,NUM_SENSORS*NUM_SAMPLES} }:
	if(in_range==true) {
		for(int i=0; i<NUM_SENSORS; i++) {
			for(int j=0; j<NUM_SAMPLES; j++) {
				test_data[i][j] = i*NUM_SAMPLES + (j+1);
			}
		}
	}
	
	// set test_data[][] to values that are all out of range:
	else {
		for(int i=0; i<NUM_SENSORS; i++) {
			for(int j=0; j<NUM_SAMPLES; j++) {
				test_data[i][j] = OUT_OF_RANGE;
			}
		}
	}
}

void test_weights_vector(int ratio_denom, int num_samples) {
	cout << "----------TESTING WEIGHTS VECTOR CREATION----------." << endl << "Weights vector: [";
	
	// weights vector:
	float weights[num_samples];
	// first weight:
	weights[0] = 1.0;
	// other weights:
	for(int i=1; i<num_samples; i++) {
		weights[i] = ((ratio_denom-1.0) * weights[i-1]) / ratio_denom;
	}
	for(int i=0; i<=num_samples-2; i++) {
		cout << weights[i] << ", ";
	}
	cout << weights[num_samples-1] << "]" << endl << endl << endl;
}

void test_weight_avg(int test_data[NUM_SENSORS][NUM_SAMPLES], bool dont_change) {
	// place an out-of-range value in last column of each row of test_data[][]:
	if(dont_change==false) {
		for(int i=0; i<NUM_SENSORS; i++) {
			test_data[i][NUM_SAMPLES-1] = OUT_OF_RANGE;
		}
	}
	
	cout << "Weighted-average distances: [";
	for(int i=0; i<=NUM_SENSORS-2; i++) {
		if(weight_avg(test_data[i], NUM_SAMPLES) == OUT_OF_RANGE) {
			cout << "OR" << ", ";
		}
		else {
			cout << weight_avg(test_data[i], NUM_SAMPLES) << ", ";
		}
	}
	if(weight_avg(test_data[NUM_SENSORS-1], NUM_SAMPLES) == OUT_OF_RANGE) {
		cout << "OR" << "]" << endl;
	}
	else {
		cout << weight_avg(test_data[NUM_SENSORS-1], NUM_SAMPLES) << "]" << endl;
	}
}

void test_calc_angle() {
	cout << "----------TESTING calc_angle() FUNCTION.----------" << endl;
	cout << "All angles: [";
	for(int i=0; i<=NUM_SENSORS-2; i++) {
		cout << calc_angle(i) << ", ";
	}
	cout << calc_angle(NUM_SENSORS-1) << "]" << endl << endl << endl;
}


int main() {
	// initialize random seed:
	srand (time(NULL));
	
	// variables for testing:
	int test_data[NUM_SENSORS][NUM_SAMPLES];
	bool in_range;
	bool dont_change;
	object_data test;
	int position = 0;
	
	
	// 1) TEST WEIGHTS VECTOR CREATION:
	/*
	int ratio_denom = 3;
	int num_samples = 4;
	test_weights_vector(ratio_denom, num_samples);
	*/
	
	
	// 2) TEST weight_avg() FUNCTION:
	/*
	cout << "----------TESTING weight_avg() FUNCTION.----------" << endl;
	// 2.1) Test with all in-range values:
	cout << "Testing with all in-range values:" << endl;
	in_range = true;
	dont_change = true;
	reset_test_data(test_data, in_range);
	test_weight_avg(test_data, dont_change);
	 
	// 2.2) Test with some out-of-range values:
	cout << "Testing with some out-of-range values:" << endl;
	in_range = true;
	dont_change = false;
	reset_test_data(test_data, true);
	test_weight_avg(test_data, dont_change);
	cout << endl << endl;
	*/
	
	//4) TEST calc_angle() FUNCTION:
	/*
	test_calc_angle();
	*/
	
	
	// 3) TEST process_ToF() FUNCTION (for NUM_SAMPLES = 1):
	cout << "----------TESTING process_ToF() FUNCTION.----------" << endl << endl;
	
	// 3.1) 0 objects:
	cout << "Testing with 0 objects:" << endl;
	in_range = false;
	reset_test_data(test_data, in_range);
	test_weight_avg(test_data, true);
	init_object_data(&test);
	test = process_ToF(test_data);
	display_object_data(test);
	cout << endl;
	
	// 3.2) 1 object in full FOV:
	cout << "Testing with 1 object in full FOV:" << endl;
	in_range = true;
	reset_test_data(test_data, in_range);
	test_weight_avg(test_data, true);
	init_object_data(&test);
	test = process_ToF(test_data);
	display_object_data(test);
	cout << endl;
	
	// 3.3) 1 3-width object in random position:
	cout << "Testing with 1 3-width object:" << endl;
	in_range = false;
	reset_test_data(test_data, in_range);
	// place 1 3-width object in a random position:
	position = rand() % (NUM_SENSORS-2);
	test_data[position][0] = position + 1;
	test_data[position+1][0] = position + 2;
	test_data[position+2][0] = position + 3;
	test_weight_avg(test_data, true);
	init_object_data(&test);
	test = process_ToF(test_data);
	display_object_data(test);
	cout << endl;
	
	// 3.4) 2 1-width objects in random position:
	cout << "Testing with 2 1-width objects:" << endl;
	in_range = false;
	reset_test_data(test_data, in_range);
	// place 2 1-width object in a random position:
	int position1 = rand() % (NUM_SENSORS-1);
	test_data[position1][0] = position1 + 1;
	bool stop = false;
	while(stop==false) {
		int position2 = rand() % (NUM_SENSORS-1);
		if(position2 != position1 and position2 != position1+1 and position2 != position1-1) {
			test_data[position2][0] = position2 + 1;
			stop = true;
		}
	}
	test_weight_avg(test_data, true);
	init_object_data(&test);
	test = process_ToF(test_data);
	display_object_data(test);
	cout << endl;
	
	// 3.5) 4 1-width objects:
	cout << "Testing with 4 1-width objects:" << endl;
	in_range = false;
	reset_test_data(test_data, in_range);
	// place 4 1-width objects in FOV:
	for(int i=0; i<NUM_SENSORS; i++) {
		if(i % 2 == 0) {
			test_data[i][0] = i + 1;
		}
	}
	test_weight_avg(test_data, true);
	init_object_data(&test);
	test = process_ToF(test_data);
	display_object_data(test);
	cout << endl;
	
	
	
	cout << endl;
    return 0;
}
