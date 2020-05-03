#include "Adafruit_VL53L0X.h"


// I2C addresses of sensors:
#define ADDRESS1 0x30
#define ADDRESS2 0x31
// shutdown pins of sensors:
#define SHUT1 2
#define SHUT2 4
#define SHUT3 9
#define SHUT4 10
#define SHUT5 11
#define SHUT6 12
#define SHUT7 13
// set DEBUG to true to get debug data for sensor ranging:
#define DEBUG false

// VL53L0X sensor objects:
Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();

// measurement variables for sensors:
#define OUT_OF_RANGE 1000
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;


/*
 * Function: setID()
 * Description:
    1) Resets all sensors by setting their shutdown pins low (for 10 ms), then sets all shutdown pins high (for 10 ms) to bring sensors out of reset mode.
    2) For each sensor:
      2.1) Keep all previous sensors active, and activate current sensor by setting its shutdown pin high.
      2.2) Shutdown all next sensors by setting their shutdown pins low.
      2.3) Initialize sensor with .begin(new_i2c_address).
        2.3.1) Address must be less than 0x7F and cannot be 0x29.
 */
void set_address() {
  // reset all sensors:
  digitalWrite(SHUT1, LOW);
  digitalWrite(SHUT2, LOW);
  // digitalWrite(SHUT3, LOW);
  // digitalWrite(SHUT4, LOW);
  delay(10);
  // bring all sensors out of reset mode:
  digitalWrite(SHUT1, HIGH);
  digitalWrite(SHUT2, HIGH);
  // digitalWrite(SHUT3, HIGH);
  // digitalWrite(SHUT4, HIGH);
  delay(10);

  // activate just sensor 1:
  digitalWrite(SHUT1, HIGH);
  digitalWrite(SHUT2, LOW);
  // initialize sensor 1:
  if(!sensor1.begin(ADDRESS1)) {
    Serial.println(F("Failed to boot sensor 1."));
    while(1);
  }
  delay(10);

  // activate just sensor 2:
  digitalWrite(SHUT2, HIGH);
  delay(10);
  // initialize sensor 2:
  if(!sensor2.begin(ADDRESS2)) {
    Serial.println(F("Failed to boot sensor 2."));
    while(1);
  }
}

void read_sensors() {
  // range sensors:
  sensor1.rangingTest(&measure1, DEBUG);
  sensor2.rangingTest(&measure2, DEBUG);

  // print sensor 1 reading:
  Serial.print("Sensor 1: ");
  if(measure1.RangeStatus != 4) {   // if in range
    Serial.print(measure1.RangeMilliMeter);
    // Serial.print(" mm");
  }
  else {    // if out of range
    // Serial.print("Out of range.");
    Serial.print(OUT_OF_RANGE);
  }
  // write CR and LF terminator characters:
  Serial.write(13);
  Serial.write(10);
  
  // Serial.print("    ");

  // print sensor 2 reading:
  Serial.print("Sensor 2: ");
  if(measure2.RangeStatus != 4) {   // if in range
    Serial.print(measure2.RangeMilliMeter);
    // Serial.print(" mm");
  }
  else {    // if out of range
    // Serial.print("Out of range.");
    Serial.print(OUT_OF_RANGE);
  }
  // write CR and LF terminator characters:
  Serial.write(13);
  Serial.write(10);
  
  // Serial.println();
}


void setup() {
  Serial.begin(115200);
  
  // wait until serial port opens (for native USB devices):
  while (!Serial) {
    delay(1);
  }

  // configure shutdown pins as outputs:
  pinMode(SHUT1, OUTPUT);
  pinMode(SHUT2, OUTPUT);
  pinMode(SHUT3, OUTPUT);
  pinMode(SHUT4, OUTPUT);
  pinMode(SHUT5, OUTPUT);
  pinMode(SHUT6, OUTPUT);
  pinMode(SHUT7, OUTPUT);

  // (temporary) write extra shutdown pins high:
  // digitalWrite(SHUT3, HIGH);
  // digitalWrite(SHUT4, HIGH);
  // digitalWrite(SHUT5, HIGH);
  // digitalWrite(SHUT6, HIGH);
  // digitalWrite(SHUT7, HIGH);

  // initialize sensors in reset mode:
  digitalWrite(SHUT1, LOW);
  digitalWrite(SHUT2, LOW);
  // Serial.println("Sensors in reset mode (pins are low).");
  
  
  // Serial.println("Setting I2C addresses of sensors...");
  set_address();
}

void loop() {
   
  read_sensors();
  // wait for 100 ms in between sensor reads:
  delay(100);
}
