/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "libjpeg.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

JPEG_HandleTypeDef hjpeg;

SD_HandleTypeDef hsd1;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

//used for console output
FILE __stdout;

/*
 * -----------------------------CAMERA COMMAND AND ACK HEX ARRAYS---------------------------
 *
 * Below are commands commonly used for interfacing with the camera, globally defined
 * only because of their ubiquity, all commands begin with 0x56 0x00 and all ACKs or returns
 * begin with 0x76 0x00, behavior after is somewhat variable but often the first five characters
 * are relatively predictable between the two going something like...
 * 		COMMAND:	0x56 0x0 0xBB 0x1 0x0
 * 		ACK:		0x76 0x0 0xBB 0x0 0x0
 *
 * 	For some commands or acknowledgements, the listed values will NOT be what you input/receive because
 * 	they depend on values like the image length, addresses, etc. So make sure that you change those areas
 * 	before using the arrays based off of what you need!
 */

// use for camera set-up
uint8_t CAM_RESET[]							=	{0x56, 0x0, 0x26, 0x0};
uint8_t CAM_RESET_ACK[]						=	{0x76, 0x0, 0x26, 0x0};

// use for camera resolution settings
uint8_t CAM_RES_640_480[]					=	{0x56, 0x0, 0x31, 0x05, 0x04, 0x01, 0x0, 0x19, 0x00};
uint8_t CAM_RES_320_240[]					=	{0x56, 0x0, 0x31, 0x05, 0x04, 0x01, 0x0, 0x19, 0x11};
uint8_t CAM_RES_ACK[]						=	{0x76, 0x0, 0x31, 0x0, 0x0};

// use for camera to take a shot
uint8_t CAM_CAPTURE[]						=	{0x56, 0x0, 0x36, 0x1, 0x0};
uint8_t CAM_CAPTURE_ACK[]					= 	{0x76, 0x0, 0x36, 0x0, 0x0};

// use for requesting image length, NOTE: in ACK the final two bytes are not 0xFF but
// are instead the high and low bytes of the length
uint8_t CAM_IMG_LENGTH[]					=	{0x56, 0x0, 0x34, 0x1, 0x0};
uint8_t CAM_IMG_LENGTH_ACK[]				=	{0x76, 0x0, 0x34, 0x0, 0x4, 0x0, 0x0, 0xFF, 0xFF};

/*
 * use these for actually getting the image data, heavy variability here
 *
 * GENERAL JPG NOTES:	all jpgs should start with 0xFF 0xD8 0xFF (unsure still on last 0xFF) and end with 0xFF 0xD9
 *
 * NOTES FOR COMMAND:	the 0xFF 0xFF sequence is actually a start address for the image data, probably default to 0x0 0x0 for future
 * 						the 0xEE 0xEE is actually meant to be image length, pulled from the results of the previous set of commands/acks
 *
 * NOTES FOR ACKS: 		listed in two parts here, the first part is the beginning of the image data and the last part is the end and cleanup
 * 						keep in mind the jpg rules! first part ends with 0xFF 0xD8 and second part starts with 0xFF 0xD9, but these are
 * 						actually parts of the jpg and should also be written to something!
 */
uint8_t CAM_READ_IMG[]						=	{0x56, 0x0, 0x32, 0x0C, 0x0, 0x0D, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0xEE, 0xEE, 0x0, 0x0A};
uint8_t CAM_READ_IMG_ACK_1[]				=	{0x76, 0x0, 0x32, 0x0, 0x0, 0xFF, 0xD8};
uint8_t CAM_READ_IMG_ACK_2[]				=	{0xFF, 0xD9, 0x76, 0x0, 0x32, 0x0, 0x0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_JPEG_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_JPEG_Init();
  MX_SDMMC1_SD_Init();
  MX_UART4_Init();
  MX_FATFS_Init();
  MX_LIBJPEG_Init();
  /* USER CODE BEGIN 2 */

  // call for initial reset state
  uint8_t set_up_ack[4];
  Camera_Setup(set_up_ack);

  //call for 320x240 resolution
  uint8_t res_ack[5];
  Camera_Set_Res(res_ack, 3);

  uint8_t prompt;
  uint8_t count = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if (prompt == 0x0)
		  printf("No I2C comms detected yet.\n");

	  HAL_Delay(50);

	  prompt = 0x0;
	  HAL_I2C_Slave_Receive(&hi2c1, &prompt, 1, HAL_MAX_DELAY);

	  printf("Prompt Value: %d\n", prompt);

	  if (prompt == 0x01) {

		  /*
		   * For now, we assume that all communication works out.
		   * There is no ACK checks for now, simple one would be to
		   * check if the first element is 0x76 as all ACKs involve
		   * that.
		   */

		  count++;
		  printf("I2C comm detected, count at: %d\n", count);

		  // get TOF distance values, 7 for the 7 sensors on the board
		  uint8_t dist_vals[7];
		  get_TOF_Data(dist_vals);

		  // get compass heading for calibration, unsure of exact format
		  uint8_t hdng_cal[1];
		  get_Compass_Data(hdng_cal);


		  /*
		   * Begin process of extracting image, then processing image.
		   *
		   * Many of the below functions have what should be final code, except
		   * for the image processing functions as those are still waiting for
		   * OpenMV adapted collateral.
		   *
		   * Some ACK checks could be added to verify information, but otherwise the
		   * flow of the below code should be mostly finalized.
		   */

		  printf("Taking snapshot...\n");
		  uint8_t snapshot_ack[5];
		  Camera_Snapshot(snapshot_ack);

		  printf("Getting image length...\n");
		  uint8_t length_ack[9];
		  Camera_Get_IMG_Length(length_ack);

		  // indices 7 and 8 of the length_ack should have actual values
		  uint16_t img_length = (uint16_t) length_ack[7]*255 + (uint16_t) length_ack[8];

		  // need to set up for reading image based on length, variable due to compression
		  CAM_READ_IMG[8] = 0x0;				// indices 8 and 9 should have the mem positions
		  CAM_READ_IMG[9] = 0x0;
		  CAM_READ_IMG[12] = length_ack[7];		// indices 12 and 13 should have the high and low byte length
		  CAM_READ_IMG[13] = length_ack[8];
		  uint8_t img_buf_and_ack[9+img_length];

		  printf("Getting image...\n");
		  Camera_Get_IMG(img_buf_and_ack, length_ack[7], length_ack[8], 0x0, 0x0);

		  // create image object for easy access to parameters, copy over jpg img data
		  image_t* cam_img = malloc(sizeof(image_t));
		  cam_img->w = 320;
		  cam_img->h = 240;
		  cam_img->bpp = 3;
		  cam_img->pixels = malloc(img_length*sizeof(uint8_t));
		  for (int i = 0; i < img_length; i++)
			  cam_img->pixels[i] = img_buf_and_ack[5+i];

		  // begin image processing, store into dynamic array of objects
		  obj_t* objects;

		  uint8_t buffer[19];
		  printf("Data collection finished, sending data...");
		  strcpy(buffer, "Message received.\n");
		  HAL_I2C_Slave_Transmit(&hi2c1, buffer, sizeof(buffer)/sizeof(buffer[0]), HAL_MAX_DELAY);
	  }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_I2C1;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x307075B1;
  hi2c1.Init.OwnAddress1 = 0x01;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00B03FDB;
  hi2c2.Init.OwnAddress1 = 0x01;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief JPEG Initialization Function
  * @param None
  * @retval None
  */
static void MX_JPEG_Init(void)
{

  /* USER CODE BEGIN JPEG_Init 0 */

  /* USER CODE END JPEG_Init 0 */

  /* USER CODE BEGIN JPEG_Init 1 */

  /* USER CODE END JPEG_Init 1 */
  hjpeg.Instance = JPEG;
  if (HAL_JPEG_Init(&hjpeg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN JPEG_Init 2 */

  /* USER CODE END JPEG_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// required set-up process for the camera to function, according to documentation
void Camera_Setup(uint8_t* rec)
{
	HAL_Delay(2500);	//2500 ms delay
	while(!send_UART(CAM_RESET, rec)) {}	//repeats until successful message sent and message received

	printf("%d\n", sizeof(rec)/sizeof(rec[0]));

	for (int i = 0; i < sizeof(rec)/sizeof(rec[0]); i++) {
		if (rec[i] != CAM_RESET_ACK[i]) {
			printf("ACK did not match expected values: %d, %d", rec[i], CAM_RESET_ACK[i]);
		}
	}
}

//orders camera to take snapshot
void Camera_Snapshot(uint8_t* rec) {
	while(!send_UART(CAM_CAPTURE, rec)) {}
}

//returns 2-value array of characters, first is high part of 16-bit length, second is low part
void Camera_Get_IMG_Length(uint8_t* rec) {
	while(!send_UART(CAM_IMG_LENGTH, rec)) {}
}

//records the image into a statically allocated array of the given
void Camera_Get_IMG(uint8_t* rec, uint8_t lengthH, uint8_t lengthL, uint8_t memH, uint8_t memL) {

	CAM_READ_IMG[12] = lengthH;
	CAM_READ_IMG[13] = lengthL;

	CAM_READ_IMG[8] = memH;
	CAM_READ_IMG[9] = memL;

	while(!send_UART(CAM_READ_IMG, rec)) {}

}

//sets the resolution of the image, full resolution likely not needed for crude processing
//0x00 is default for 640x480, to get 320x240 must manually send command, replace 0x00 with 0x11
void Camera_Set_Res(uint8_t* rec, uint8_t res) {

	// for 640x480
	if (res == 6)
		while(!send_UART(CAM_RES_640_480, rec)) {}

	// for 320x240
	else if (res == 3)
		while(!send_UART(CAM_RES_320_240, rec)) {}


// dummy functions to be used as interfaces to other blocks
void get_TOF_Data(uint8_t* buf) {

}

void get_Compass_Data(uint8_t* buf) {

}

// abstracted function for general USART exchange, note that the size of rec should be known beforehand
bool send_UART(uint8_t* comm, uint8_t* rec)
{
	int i = 0;
	while(HAL_UART_Transmit(&huart4, comm, sizeof(comm)/sizeof(comm[0]), HAL_MAX_DELAY) != HAL_OK)
	{
		if (i > 10)
			return false;
		i++;

	}

	i = 0;
	while(HAL_UART_Receive(&huart4, rec, sizeof(rec)/sizeof(rec[0]), HAL_MAX_DELAY) != HAL_OK)
	{
		if (i > 10)
			return false;
		i++;
	}

	return true;
}

/*
 * _write is built for an override of stdio's access to the default weak _write function,
 * used for printf, this implementation with Instrument Trace Macrocell (ITM) writing enables
 * console outputs as long as the Serial Wire Viewer (SWV) configurations are correct
 *
 * NOTES FOR SWV CONFIG:
 * 		- enable SWV ITM Data Console in Windows
 * 		- enable whichever ports necessary, can leave memory as default, check console settings here
 * 		- ensure that debug settings are enabled as such that SWV is enabled, probe is GDB server, and
 * 			clock matches the SYSCLK, not HCLK
 */
int _write(int file, char* out, int len) {
	for (int i = 0; i < len; i++) {
		ITM_SendChar(out[i]);
	}
	return len;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
