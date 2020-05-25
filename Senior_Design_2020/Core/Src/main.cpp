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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main_board_comm.h"
#include "HAL_VL53L0X.h"
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
I2C_HandleTypeDef hi2c3;

JPEG_HandleTypeDef hjpeg;

UART_HandleTypeDef huart8;

/* USER CODE BEGIN PV */
HAL_VL53L0X sensor[7];

// to take in frames from the master
uint8_t master_comm_buff[9];

uint8_t buff_list_size = 4;
buffer_link buffer_list[4];

obj_t object_list[8];
uint8_t object_num = 0;

// size of this buffer is somewhat arbitrary, first dimension is essentially maximum objects - 1 so this can be scaled
uint8_t i2c_buffer[8][9];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_JPEG_Init(void);
static void MX_UART8_Init(void);
static void MX_I2C3_Init(void);
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
  MX_JPEG_Init();
  MX_UART8_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  printf("Beginning ToF initialization...\n");

   HAL_GPIO_WritePin(GPIOA, SH1_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, SH2_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, SH3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, SH4_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOC, SH5_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOC, SH6_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOC, SH7_Pin, GPIO_PIN_RESET);

   HAL_Delay(10);


   HAL_GPIO_WritePin(GPIOA, SH1_Pin, GPIO_PIN_SET);
   if (!sensor[0].begin(TOF_ADD1, false, &hi2c1))
 	  printf("Begin statement 1 failed.\n");

   HAL_GPIO_WritePin(GPIOA, SH2_Pin, GPIO_PIN_SET);
   if (!sensor[1].begin(TOF_ADD2, false, &hi2c1))
 	  printf("Begin statement 2 failed.\n");

   HAL_GPIO_WritePin(GPIOA, SH3_Pin, GPIO_PIN_SET);
   if (!sensor[2].begin(TOF_ADD3, false, &hi2c1))
 	  printf("Begin statement 3 failed.\n");

   HAL_GPIO_WritePin(GPIOA, SH4_Pin, GPIO_PIN_SET);
   if (!sensor[3].begin(TOF_ADD4, false, &hi2c1))
 	  printf("Begin statement 4 failed.\n");

   HAL_GPIO_WritePin(GPIOC, SH5_Pin, GPIO_PIN_SET);
   if (!sensor[4].begin(TOF_ADD5, false, &hi2c1))
 	  printf("Begin statement 5 failed.\n");

   HAL_GPIO_WritePin(GPIOC, SH6_Pin, GPIO_PIN_SET);
   if (!sensor[5].begin(TOF_ADD6, false, &hi2c1))
 	  printf("Begin statement 6 failed.\n");

   HAL_GPIO_WritePin(GPIOC, SH7_Pin, GPIO_PIN_SET);
   if (!sensor[6].begin(TOF_ADD7, false, &hi2c1))
 	  printf("Begin statement 7 failed.\n");



   printf("Finished ToF initialization thus far.\n");

   // initialize camera before anything else with desired resolution, amount of compression (0x0 to 0xFF), and baudrate
   // note: current issue exists, it appears that attempting to change the baud rate will change the camera's baud rate, but our UART will stay
   //		at 38400 baud, we lose out on 3x speedup because of this but we are unsure of how to fix
   Camera_Initialize(&huart8, 1, 0xFF, 38400);
   HAL_Delay(10);

   //initialization of frameptr, must be incremented to request images correctly
   uint16_t frameptr = 0;

   HAL_StatusTypeDef result = HAL_ERROR;

   // initialization of circular buffer
   for (int i = 0; i < buff_list_size-1; i++) {
    buffer_list[i].jpeg_size = 0;
    buffer_list[i].next = buffer_list + i + 1;
   }
   buffer_list[buff_list_size-1].jpeg_size = 0;
   buffer_list[buff_list_size-1].next = buffer_list;

   buffer_link* curr_link = buffer_list;
   buffer_link* transmit_link = NULL;

  //HAL_I2C_Slave_Receive_IT(&hi2c3, master_comm_buff, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
 	  printf("Beginning while loop.\n");

 	  // begin with ToF data collection for the image we are about to take a snapshot of
 	  // note: currently API does not function, so this function does not work, but its place in the flow should be around here
	  get_TOF_Data();


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

	  HAL_Delay(100);
	  printf("Taking snapshot...\n");
	  uint8_t snapshot_ack[5];
	  uint8_t *snapshot_ack1 = snapshot_ack;
	  Camera_Snapshot(&huart8, snapshot_ack1, sizeof(snapshot_ack));

	  if (snapshot_ack[0] == 0x76)
		  printf("Ack for snapshot received.\n");
	  else
		  printf("No ack received for snapshot.\n");

	  printf("Getting image length...\n");
	  uint8_t length_ack[9];
	  Camera_Get_IMG_Length(&huart8, length_ack, sizeof(length_ack));

	  if (length_ack[0] == 0x76)
		  printf("Ack for image length received.\n");
	  else
		  printf("No ack received for image length.\n");

	  // indices 7 and 8 of the length_ack should have actual values
	  uint16_t img_length = (((uint16_t) length_ack[7]) << 8) + (uint16_t) length_ack[8];
	  printf("Found image length: %lu\n", img_length);




	  curr_link->jpeg_size = img_length;

	  uint32_t ticktransfer1 = HAL_GetTick();

	  printf("Getting image...\n");
	  Camera_Get_IMG(&huart8, curr_link->jpeg, (10+img_length), length_ack[7], length_ack[8], (uint8_t) (frameptr >> 8), (uint8_t) frameptr);

	  uint32_t ticktransfer2 = HAL_GetTick();

	  printf("Image transfer time: %lu\n", ticktransfer2 - ticktransfer1);

	  uint8_t stop_ack[5];
	  uint8_t* stop_ack1 = stop_ack;
	  Camera_Stop_Capture(&huart8, stop_ack1, sizeof(stop_ack));

	  if (stop_ack[0] == 0x76)
		  printf("Ack for stopping capture received.\n");
	  else
		  printf("No ack received for stopping capture.\n");


	  uint8_t* jpeg_img_buf = curr_link->jpeg;
	  jpeg_img_buf += 5;


	  //below is a test for the I2C interface
/*

	  // image processed successfully
	  uint8_t comm = 20;
	  // something did not process correctly
	  uint8_t comm_b = 25;

	  result = HAL_I2C_Slave_Receive(&hi2c2, master_comm_buff, 1, 3000);

	  if (result == 0x0 && master_comm_buff[0] == 0x1) {
		  if (stop_ack[0] == 0x76)
			  result = HAL_I2C_Slave_Transmit(&hi2c2, &comm, 1, 3000);
		  else
			  result = HAL_I2C_Slave_Transmit(&hi2c2, &comm_b, 1, 3000);
	  }

	  else if (result == 0x0 && master_comm_buff[0] == 0x2) {
		  uint16_t jpeg_rem = transmit_link->jpeg_size + 5;
		  while (jpeg_rem > 0 && jpeg_rem < 3500) {
			  if (jpeg_rem/200 > 0)
				  result = HAL_I2C_Slave_Transmit(&hi2c2, (transmit_link->jpeg + 5 + transmit_link->jpeg_size - jpeg_rem), 200, 2000);
			  else {
				  result = HAL_I2C_Slave_Transmit(&hi2c2, (transmit_link->jpeg + 5 + transmit_link->jpeg_size - 100), 200, 2000);
				  printf("At last part.\n");
			  }

			  jpeg_rem -= 200;
			  printf("jpeg_rem: %d\n", jpeg_rem);

			  if (result != HAL_OK)
				  printf("Transmission did not finish, result status: %d\n", result);

			  //HAL_Delay(10);
		  }
	  }
*/

	  uint8_t red_mask = 0b11111000;
	  uint8_t blue_mask = 0b00011111;
	  uint8_t greenh_mask = 0b00000111;
	  uint8_t greenl_mask = 0b11100000;

	  curr_link->rgb[11200] = 255;
	  uint32_t tickj = HAL_GetTick();
	  result = HAL_JPEG_Decode(&hjpeg, jpeg_img_buf, (uint32_t) curr_link->jpeg_size, curr_link->rgb, 57600, 3000);

	  uint8_t* buff_check = curr_link->rgb;
	  buff_check++;
	  buff_check++;
	  buff_check++;
	  buff_check++;
	  buff_check++;

	  if (result == HAL_OK) {
		  printf("JPEG conversion finished successfully.\n");

		  //seems that output is only RGB565, further testing needed but RGB888 is unresponsive and output is 2/3 of expected size
		  for (uint64_t i = 0; i < 80; i++) {
			  //printf("{%d, %d} ", curr_link->rgb[19200+2*i], curr_link->rgb[19200+2*i+1]);

			  uint16_t red_v = ((red_mask & curr_link->rgb[19200+2*i+1]) >> 3)*255/31;
			  uint16_t blue_v = ((blue_mask & curr_link->rgb[19200+2*i]))*255/31;
			  uint16_t greenl_v = (greenl_mask & curr_link->rgb[19200+2*i]) >> 5;
			  uint16_t greenh_v = (greenh_mask & curr_link->rgb[19200+2*i+1]) << 3;
			  uint16_t green_v = (greenl_v + greenh_v)*255/63;

			  printf("{%d, %d, %d} ", red_v, green_v, blue_v);
		  }
		  printf("\n");
	  }
	  else
		  printf("JPEG conversion failed.\n");

	  uint32_t tickj2 = HAL_GetTick();
	  printf("JPEG time elapsed: %lu\n", tickj2-tickj);

	  // iterate to avoid sending repeats
	  transmit_link = curr_link;
	  curr_link = curr_link->next;

	  printf("\n\nLOOP COMPLETED\n\n");

	  printf("Result: %d\n\n", result);
	  result = HAL_ERROR;
	  HAL_Delay(10);


	  /*
	   *
	   * PLACEMENT OF COMPUTER VISION ALGORITHM IN FLOW
	   *
	   */


	  master_comm_buff[0] = 20;
	  master_comm_buff[1] = 30;
	  master_comm_buff[2] = 40;
	  master_comm_buff[3] = 50;
	  // placeholder blocking i2c communication until interrupt driven interface is debugged
	  result = HAL_I2C_Slave_Receive(&hi2c3, master_comm_buff, 9, 2000);

	  if (master_comm_buff[0] == DATA_REQ) {
		  package_data(object_list, object_num, i2c_buffer);
		  for (int i = 0; i < object_num/2 + object_num%2; i++)
			  result = HAL_I2C_Slave_Transmit(&hi2c3, i2c_buffer[i], 9, 2000);
	  }
	  result = HAL_ERROR;

	  //printf("End of while loop reached...");

    /* USER CODE END WHILE */

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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART8|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_I2C1;
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
  hi2c1.Init.OwnAddress1 = 4;
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x307075B1;
  hi2c3.Init.OwnAddress1 = 8;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
  * @brief UART8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART8_Init(void)
{

  /* USER CODE BEGIN UART8_Init 0 */

  /* USER CODE END UART8_Init 0 */

  /* USER CODE BEGIN UART8_Init 1 */

  /* USER CODE END UART8_Init 1 */
  huart8.Instance = UART8;
  huart8.Init.BaudRate = 38400;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART8_Init 2 */

  /* USER CODE END UART8_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SH7_Pin|SH6_Pin|SH5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SH4_Pin|SH3_Pin|SH2_Pin|SH1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SH7_Pin SH6_Pin SH5_Pin */
  GPIO_InitStruct.Pin = SH7_Pin|SH6_Pin|SH5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SH4_Pin SH3_Pin SH2_Pin SH1_Pin */
  GPIO_InitStruct.Pin = SH4_Pin|SH3_Pin|SH2_Pin|SH1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */



// dummy functions to be used as interfaces to other blocks
void get_TOF_Data() {

	VL53L0X_RangingMeasurementData_t measure;

	int data[7][5];
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 7; j++) {
			sensor[j].rangingTest(&measure, true);
			data[j][i] = measure.RangeMilliMeter;
		}
	}

	process_ToF(object_list, &object_num, data);
}


// attempt at dealing with UART timing issues, could be used for the future for a UART interface driven by interrupt
// especially should be considered because of current baud rate change issues
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	//printf("Interrupt triggered.");

}

// for when we are triggered as slave
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef * hi2c) {
	uint8_t message = 1;

	if (master_comm_buff[0] == 0x01)
		HAL_I2C_Slave_Transmit(hi2c, &message, 1, HAL_MAX_DELAY);

	printf("In call back.\n");
	HAL_I2C_Slave_Receive_IT(hi2c, master_comm_buff, 1);
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

/*
 * Due to issues with the speed of HAL_UART functions, this is a substitute that strips away much of the safety measures that the HAL drivers
 * implement. It is fairly minimal. The variables passed are still in line with what a typical UART HAL driver might expect.
 *
 * pdata8bits is used explicitly for instances where 8 bit UART is expected. Any parity UART would require a rewrite of this function.
 * One could simply expand it with pdata16bits as a uint16_t*
 */
HAL_StatusTypeDef UART_Fast_Transmit(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size, uint32_t Timeout) {

	uint32_t tickstart;
	tickstart = HAL_GetTick();
	uint8_t* pdata8bits = pData;

    huart->TxXferSize  = Size;
    huart->TxXferCount = Size;

    while (huart->TxXferCount > 0U)
    {
      //ensure that Tx operations are finished before we continue
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      huart->Instance->TDR = (uint8_t)(*pdata8bits & 0xFFU); // what is the purpose of the & operation if the mask is just 0xFF
      pdata8bits++;
      huart->TxXferCount--;
    }

	return HAL_OK;
}

HAL_StatusTypeDef UART_Fast_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout) {

	uint32_t tickstart;
	tickstart = HAL_GetTick();
	uint8_t* pdata8bits = pData;

    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;

    while (huart->RxXferCount > 0U)
    {
       /*if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
       {
         return HAL_TIMEOUT;
       }*/

       while((__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) ? SET : RESET) == RESET) {} // ? operator is a conditional ternary, may just be fast

       *pdata8bits = (uint8_t)(huart->Instance->RDR & 0xFFU);	// note that UART mask here is variable in HAL drivers, often 255 so that is default
       pdata8bits++;

       huart->RxXferCount--;
    }

    return HAL_OK;

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
