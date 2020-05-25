#ifndef HAL_VL53L0X_H
#define HAL_VL53L0X_H

#include <vl53l0x_api.h>

#define VL53L0X_I2C_ADDR 0x29 ///< Default sensor I2C address

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with VL53L0X
   time-of-flight sensor chips
*/
/**************************************************************************/
class HAL_VL53L0X {
public:
  bool begin(uint8_t i2c_addr = VL53L0X_I2C_ADDR, bool debug = false,
		  I2C_HandleTypeDef* hi2c = NULL);
  bool setAddress(uint8_t newAddr);

  /**************************************************************************/
  /*!
      @brief  get a ranging measurement from the device
      @param  pRangingMeasurementData the pointer to the struct the data will be
     stored in
      @param debug Optional debug flag. If true debug information will print via
     Serial.print during execution. Defaults to false.
      @returns True if address was set successfully, False otherwise
  */
  /**************************************************************************/
  VL53L0X_Error
  rangingTest(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
              bool debug = false) {
    return getSingleRangingMeasurement(pRangingMeasurementData, debug);
  };

  VL53L0X_Error getSingleRangingMeasurement(
      VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
      bool debug = false);
  void
  printRangeStatus(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData);

  VL53L0X_Error Status =
      VL53L0X_ERROR_NONE; ///< indicates whether or not the sensor has
                          ///< encountered an error

private:
  VL53L0X_Dev_t MyDevice;
  VL53L0X_Dev_t *pMyDevice = &MyDevice;
  VL53L0X_Version_t Version;
  VL53L0X_Version_t *pVersion = &Version;
  VL53L0X_DeviceInfo_t DeviceInfo;
};

#endif
