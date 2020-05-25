#include <vl53l0x_def.h>
#include <vl53l0x_i2c_platform.h>

//#define I2C_DEBUG

int VL53L0X_i2c_init(I2C_HandleTypeDef* hi2c) {

  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata,
                        uint32_t count, I2C_HandleTypeDef* hi2c) {
	HAL_StatusTypeDef res;
	res = HAL_I2C_Master_Transmit(hi2c, deviceAddress << 1, &index, 1, 100);
#ifdef I2C_DEBUG
  printf("Writing index: %d\nResult: %d\n", index, res);
#endif
  	res = HAL_I2C_Master_Transmit(hi2c, deviceAddress << 1, pdata, count, 100);
#ifdef I2C_DEBUG
  	printf("Finished sending message, result: %d\n", res);
#endif

  return VL53L0X_ERROR_NONE;
}

int VL53L0X_read_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata,
                       uint32_t count, I2C_HandleTypeDef* hi2c) {
	HAL_StatusTypeDef res;
	pdata[0] = 0;
	res = HAL_I2C_Master_Transmit(hi2c, deviceAddress << 1, &index, 1, 25);
#ifdef I2C_DEBUG
  printf("Writing index: %d\nResult: %d\n", index, res);
#endif

  res = HAL_I2C_Master_Receive(hi2c, deviceAddress << 1, pdata, count, 25);
#ifdef I2C_DEBUG
  printf("Finished receiving message, result: %d\n", res);
#endif

  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_byte(uint8_t deviceAddress, uint8_t index, uint8_t data,
		I2C_HandleTypeDef* hi2c) {
  return VL53L0X_write_multi(deviceAddress, index, &data, 1, hi2c);
}

int VL53L0X_write_word(uint8_t deviceAddress, uint8_t index, uint16_t data,
		I2C_HandleTypeDef* hi2c) {
  uint8_t buff[2];
  buff[1] = data & 0xFF;
  buff[0] = data >> 8;
  return VL53L0X_write_multi(deviceAddress, index, buff, 2, hi2c);
}

int VL53L0X_write_dword(uint8_t deviceAddress, uint8_t index, uint32_t data,
		I2C_HandleTypeDef* hi2c) {
  uint8_t buff[4];

  buff[3] = data & 0xFF;
  buff[2] = data >> 8;
  buff[1] = data >> 16;
  buff[0] = data >> 24;

  return VL53L0X_write_multi(deviceAddress, index, buff, 4, hi2c);
}

int VL53L0X_read_byte(uint8_t deviceAddress, uint8_t index, uint8_t *data,
		I2C_HandleTypeDef* hi2c) {
  return VL53L0X_read_multi(deviceAddress, index, data, 1, hi2c);
}

int VL53L0X_read_word(uint8_t deviceAddress, uint8_t index, uint16_t *data,
		I2C_HandleTypeDef* hi2c) {
  uint8_t buff[2];
  int r = VL53L0X_read_multi(deviceAddress, index, buff, 2, hi2c);

  uint16_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  *data = tmp;

  return r;
}

int VL53L0X_read_dword(uint8_t deviceAddress, uint8_t index, uint32_t *data,
		I2C_HandleTypeDef* hi2c) {
  uint8_t buff[4];
  int r = VL53L0X_read_multi(deviceAddress, index, buff, 4, hi2c);

  uint32_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  tmp <<= 8;
  tmp |= buff[2];
  tmp <<= 8;
  tmp |= buff[3];

  *data = tmp;

  return r;
}
