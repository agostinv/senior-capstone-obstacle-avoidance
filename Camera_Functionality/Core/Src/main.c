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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

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

JPEG_HandleTypeDef hjpeg;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
uint8_t junk[255];

uint8_t img_buf[67200];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
static void MX_JPEG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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
uint8_t CAM_RESET[]							=	{0x56, 0x0, 0x26, 0x0, 0x0};
uint8_t CAM_RESET_ACK[]						=	{0x76, 0x0, 0x26, 0x0, 0x0};

// use for camera resolution settings
uint8_t CAM_RES_640_480[]					=	{0x56, 0x0, 0x31, 0x05, 0x04, 0x01, 0x0, 0x19, 0x00};
uint8_t CAM_RES_320_240[]					=	{0x56, 0x0, 0x31, 0x05, 0x04, 0x01, 0x0, 0x19, 0x11};
uint8_t CAM_RES_160_120[]					=	{0x56, 0x0, 0x31, 0x05, 0x04, 0x01, 0x0, 0x19, 0x22};
uint8_t CAM_RES_ACK[]						=	{0x76, 0x0, 0x31, 0x0, 0x0};

// use to compress the image further than normal, typical value is at 0x36, can be pushed up to 0xFF
// acknowledgement is the same as resolution ack
uint8_t CAM_COMPRESS[]						=	{0x56, 0x0, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x36};
uint8_t CAM_COMPRESS_ACK[] 					=	{0x76, 0x0, 0x31, 0x0, 0x0};

// use to change camera baud rate, note that you must init your UART again to properly communicate
// default baud rate of camera is 38400 baud
uint8_t CAM_BAUD_9600[]						=	{0x56, 0x0, 0x24, 0x03, 0x01, 0xAE, 0xC8};
uint8_t CAM_BAUD_19200[]					=	{0x56, 0x0, 0x24, 0x03, 0x01, 0x56, 0xE4};
uint8_t CAM_BAUD_38400[]					= 	{0x56, 0x0, 0x24, 0x03, 0x01, 0x2A, 0xF2};
uint8_t CAM_BAUD_57600[]					=	{0x56, 0x0, 0x24, 0x03, 0x01, 0x1C, 0x1C};
uint8_t CAM_BAUD_115200[]					=	{0x56, 0x0, 0x24, 0x03, 0x01, 0x0D, 0xA6};
uint8_t CAM_BAUD_ACK[]						=	{0x76, 0x0, 0x24, 0x0, 0x0};

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
 * NOTES FOR COMMAND:	the 0xFF 0xFF sequence is actually a start address for the image data, needs to be set to a frame pointer that shifts by the
 * 						image's length
 *
 * 						the 0xEE 0xEE is actually meant to be image length, pulled from the results of the previous set of commands/acks
 *
 * 						the 0x0D can potentially be 0x0A instead, Adafruit uses the latter while another reference sheet uses the former
 *
 * 						finally, the end 0x0 0x0A is actually the "camera delay," it may be variable based off of model but I am unsure and have no other
 * 						models to test on
 *
 *
 * NOTES FOR ACKS: 		listed in two parts here, the first part is the beginning of the image data and the last part is the end and cleanup
 * 						keep in mind the jpg rules! first part ends with 0xFF 0xD8 and second part starts with 0xFF 0xD9, but these are
 * 						actually parts of the jpg and should also be written to something!
 */
uint8_t CAM_READ_IMG[]						=	{0x56, 0x0, 0x32, 0x0C, 0x0, 0x0A, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0xEE, 0xEE, 0x0, 0x0A};
uint8_t CAM_READ_IMG_ACK_1[]				=	{0x76, 0x0, 0x32, 0x0, 0x0};
uint8_t CAM_READ_IMG_ACK_2[]				=	{0x76, 0x0, 0x32, 0x0, 0x0};
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


  //Camera_Setup(CAM_RESET);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  MX_JPEG_Init();
  /* USER CODE BEGIN 2 */
  printf("%d\n", sizeof(CAM_RESET));
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_SET);

 /* HAL_Delay(100);

  uint32_t tick1 = HAL_GetTick();
  uint32_t count = 0;
  uint8_t* array = (uint8_t*) malloc(sizeof(uint8_t)*160*120);
  uint32_t tick2 = HAL_GetTick();
  printf("Malloc run-time... %lu\n", tick2-tick1);

  uint32_t tick3 = HAL_GetTick();
  // quick and dirty timing test for Shanti
  for (int i = 0; i < 120; i++)
	  for (int j = 0; j < 160; j++) {
		  array[j+i*j] = count/20;
		  count++;
	  }
  uint32_t tick4 = HAL_GetTick();
  printf("Loop run-time... %lu\n", tick4-tick3);

  free(array);
  get_TOF_Data(array);

  uint32_t tick5 = HAL_GetTick();
  printf("Altogether run-time, including free... %lu\n", tick5-tick1);

  */

  img_buf[0] = 0;
  uint8_t test = 0b10101010;
  uint8_t test1 = 200;
  uint8_t rec1[26] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t* rec = rec1;

  //HAL_UART_Receive_IT(&huart4, rec, 5);
  //HAL_UART_Transmit(&huart4, &test, 1, 1000);
  /*send_UART(&huart4, &test, rec, 1, 26);

  for (int i = 0; i < 26; i++)
	  printf("%u\n", rec1[i]);

  send_UART(&huart4, &test1, rec, 1, 26);
  //HAL_UART_Receive_IT(&huart4, rec, 5);
  //HAL_UART_Transmit(&huart4, &test, 1, 1000);

  for (int i = 0; i < 26; i++)
	  printf("%u\n", rec[i]);
  //HAL_UART_Transmit(&huart5, &test, 1, 1000);
*/


  // initialize camera before anything else with desired resolution, amount of compression (0x0 to 0xFF), and baudrate
  Camera_Initialize(1, 0xFF, 115200);
  HAL_Delay(10);

  uint16_t frameptr = 0;

  HAL_StatusTypeDef result = 255;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  	  printf("Beginning while loop.\n");
  	  /*
	   * For now, we assume that all communication works out.
	   * There is no ACK checks for now, simple one would be to
	   * check if the first element is 0x76 as all ACKs involve
	   * that.
	   */

	  // get TOF distance values, 7 for the 7 sensors on the board
	  uint8_t dist_vals[7];
	  get_TOF_Data(dist_vals);


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

	  HAL_Delay(10);
	  printf("Taking snapshot...\n");
	  uint8_t snapshot_ack[5];
	  uint8_t *snapshot_ack1 = snapshot_ack;
	  Camera_Snapshot(snapshot_ack1, sizeof(snapshot_ack));

	  if (snapshot_ack[0] == 0x76)
		  printf("Ack for snapshot received.\n");
	  else
		  printf("No ack received for snapshot.\n");

	  printf("Getting image length...\n");
	  uint8_t length_ack[9];
	  Camera_Get_IMG_Length(length_ack, sizeof(length_ack));

	  if (length_ack[0] == 0x76)
		  printf("Ack for image length received.\n");
	  else
		  printf("No ack received for image length.\n");

	  // indices 7 and 8 of the length_ack should have actual values
	  uint16_t img_length = (((uint16_t) length_ack[7]) << 8) + (uint16_t) length_ack[8];
	  printf("Found image length: %lu\n", img_length);

	  //image buffer
	  uint8_t img_buf_and_ack[10+img_length];

	  uint32_t ticktransfer1 = HAL_GetTick();

	  printf("Getting image...\n");
	  Camera_Get_IMG(img_buf_and_ack, (10+img_length), length_ack[7], length_ack[8], (uint8_t) (frameptr >> 8), (uint8_t) frameptr);

	  uint32_t ticktransfer2 = HAL_GetTick();

	  printf("Image transfer time: %lu\n", ticktransfer2 - ticktransfer1);

	  // extremely important, do not forget to increment your frameptr by the number of bytes you just received
	  frameptr += img_length;
	  uint8_t* jpeg_img_buf = img_buf_and_ack;
	  jpeg_img_buf += 5;

	  result = HAL_JPEG_Decode(&hjpeg, jpeg_img_buf, (uint32_t) img_length, img_buf, 67200, 3000);

	  uint8_t* buff_check = img_buf;
	  buff_check++;
	  buff_check++;
	  buff_check++;
	  buff_check++;
	  buff_check++;

	  uint32_t tickj = HAL_GetTick();

	  if (result == HAL_OK) {
		  printf("JPEG conversion finished successfully.\n");

		  for (uint64_t i = 0; i < 140; i++) {
			  printf("{%d, %d, %d} ", img_buf[31350+3*i], img_buf[31350+3*i+1], img_buf[31350+3*i+2]);
		  }
		  printf("\n");
	  }
	  else
		  printf("JPEG conversion failed.\n");

	  uint32_t tickj2 = HAL_GetTick();
	  printf("JPEG time elapsed: %lu\n", tickj2-tickj);

	  // create image object for easy access to parameters, copy over jpg img data
	  /*image_t* cam_img = (image_t*) malloc(sizeof(image_t));
	  cam_img->w = 320;
	  cam_img->h = 240;
	  cam_img->bpp = 3;
	  cam_img->pixels = (uint8_t*) malloc(img_length*sizeof(uint8_t));
	  for (int i = 0; i < img_length; i++)
		  cam_img->pixels[i] = img_buf_and_ack[5+i];*/

	  printf("\n\nLOOP COMPLETED\n\n");
	  //printf("End of while loop reached, deleting and then entering delay before starting again...");


	  /*for (int i = 0; i < 2; i++) {
		  HAL_Delay(1000);
		  printf(".");
	  }
	  printf("\n");*/



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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
  huart4.Init.BaudRate = 38400;
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
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart4) != HAL_OK)
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PF2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PF3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// required set-up process for the camera to function, according to documentation
void Camera_Reset(uint8_t* rec, uint8_t len_rec)
{
	printf("Size of receiving buffer: %d\n", len_rec);
	HAL_Delay(2500);	//2500 ms delay
	send_UART(&huart4, CAM_RESET, rec, sizeof(CAM_RESET), len_rec);

	for (int i = 0; i < len_rec; i++) {
		printf("%d ", rec[i]);
	}
	printf("\n");

	HAL_Delay(1000);
}

// abstracted function for general USART exchange, note that size of rec must be known beforehand for this to work
bool send_UART(UART_HandleTypeDef* huart, uint8_t* comm, uint8_t* rec, uint8_t len_comm, uint16_t len_rec)
{
	HAL_StatusTypeDef res = 0;
	res = HAL_UART_Receive(&huart4, junk, 255, 2);
	/*for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
	printf("\n");*/
	rec[0] = 255;
	rec[1] = 10;
	rec[2] = 30;
	rec[3] = 40;
	rec[4] = 50;

	uint32_t tick3 = HAL_GetTick();

	// fastest tx operation?
	huart->TxXferCount = len_comm;
	huart->TxXferSize = len_comm;
	while (huart->TxXferCount > 0U)
	    {
	      //ensure that Tx operations are finished before we continue
	      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tick3, 1000) != HAL_OK)
	      {
	        return false;
	      }

	      huart->Instance->TDR = (uint8_t)(*comm & 0xFFU); // what is the purpose of the & operation if the mask is just 0xFF
	      comm++;
	      huart->TxXferCount--;
	    }


	// fastest rx operation, could make slightly faster on flag check
	huart->RxXferCount = len_rec;
	huart->RxXferSize  = len_rec;
	uint8_t flag = 0;
	uint8_t* pData = rec;
	uint32_t tickstart = HAL_GetTick();
	UART_MASK_COMPUTATION(huart);
	while (huart->RxXferCount > 0U)
	    {
	       if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, 5000) != HAL_OK)
	       {
	    	 printf("\n\nBROKE LOOP ON RX, TIMEOUT AT %d ITERATION\n\n", len_rec - huart->RxXferCount);
	         return HAL_TIMEOUT;
	       }

	       /*while((__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) ? SET : RESET) == RESET) {
	    	   uint32_t tick_curr = HAL_GetTick();
	    	   if (tick_curr - tick3 > 1000) {
	    		   printf("Failed at %lu iteration.\n", len_rec-huart->RxXferCount);
	    		   flag = 1;
	    		   break;
	    	   }
	       } // ? operator is a conditional ternary, may just be fast

	       if (flag == 1) {
	    	   break;
	       }*/

	       *pData = (uint8_t)(huart->Instance->RDR & (uint8_t) huart->Mask);	// note that UART mask here is variable in HAL drivers, often 255 so that is default
	       pData++;

	       huart->RxXferCount--;
	       //__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_RXNE);
	    }

	res = HAL_UART_Receive(&huart4, junk, 255, 2);
}

//orders camera to take snapshot
void Camera_Snapshot(uint8_t* rec, uint8_t len_rec) {
	send_UART(&huart4, CAM_CAPTURE, rec, sizeof(CAM_CAPTURE), len_rec);

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");

	HAL_Delay(1);
}

//returns 2-value array of characters, first is high part of 16-bit length, second is low part
void Camera_Get_IMG_Length(uint8_t* rec, uint8_t len_rec) {
	send_UART(&huart4, CAM_IMG_LENGTH, rec, sizeof(CAM_IMG_LENGTH), len_rec);

	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");
}

//records the image into a statically allocated array of the given
void Camera_Get_IMG(uint8_t* rec, uint16_t len_rec, uint8_t lengthH, uint8_t lengthL, uint8_t memH, uint8_t memL) {

	CAM_READ_IMG[12] = lengthH;
	CAM_READ_IMG[13] = lengthL;

	CAM_READ_IMG[8] = memH;
	CAM_READ_IMG[9] = memL;

	send_UART(&huart4, CAM_READ_IMG, rec, sizeof(CAM_READ_IMG), len_rec);

	/*printf("IMAGE:\n\n");
	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");*/

}

//sets the resolution of the image, full resolution likely not needed for crude processing
//0x00 is default for 640x480, to get 320x240 must manually send command, replace 0x00 with 0x11
// resolution = 6 sends 640x480, resolution = 3 sends 320x240, resolution = 1 sends 160x120
void Camera_Set_Res(uint8_t* rec, uint8_t len_rec, uint8_t res) {

	// for 640x480
	if (res == 6)
		send_UART(&huart4, CAM_RES_640_480, rec, sizeof(CAM_RES_640_480), len_rec);

	// for 320x240
	else if (res == 3)
		send_UART(&huart4, CAM_RES_320_240, rec, sizeof(CAM_RES_320_240), len_rec);

	// for 160x120
	else if (res == 1)
		send_UART(&huart4, CAM_RES_160_120, rec, sizeof(CAM_RES_160_120), len_rec);

	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");
}

// function to ease customization of compression, higher compression is vastly preferable
void Camera_Set_Compress(uint8_t* rec, uint8_t len_rec, uint8_t comp) {
	CAM_COMPRESS[8] = comp;

	send_UART(&huart4, CAM_COMPRESS, rec, sizeof(CAM_COMPRESS), len_rec);

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");
}

// function to set the baud rate, pass the baud rate directly for check
void Camera_Set_Baud(uint8_t* rec, uint8_t len_rec, uint32_t baud) {

	if (baud == 9600) {
		send_UART(&huart4, CAM_BAUD_9600, rec, sizeof(CAM_BAUD_9600), len_rec);
		HAL_UART_DeInit(&huart4);
		UART_Init_Baud(&huart4, UART4, 9600);
	}

	else if (baud == 19200) {
		send_UART(&huart4, CAM_BAUD_19200, rec, sizeof(CAM_BAUD_19200), len_rec);
		HAL_UART_DeInit(&huart4);
		UART_Init_Baud(&huart4, UART4, 19200);
	}

	else if (baud == 38400) {
		send_UART(&huart4, CAM_BAUD_38400, rec, sizeof(CAM_BAUD_38400), len_rec);
		HAL_UART_DeInit(&huart4);
		UART_Init_Baud(&huart4, UART4, 38400);
	}

	else if (baud == 57600) {
		send_UART(&huart4, CAM_BAUD_57600, rec, sizeof(CAM_BAUD_57600), len_rec);
		HAL_UART_DeInit(&huart4);
		UART_Init_Baud(&huart4, UART4, 57600);
	}

	else if (baud == 115200) {
		send_UART(&huart4, CAM_BAUD_115200, rec, sizeof(CAM_BAUD_115200), len_rec);
		HAL_UART_DeInit(&huart4);
		UART_Init_Baud(&huart4, UART4, 115200);
	}

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");
}

void Camera_Initialize(uint8_t resolution, uint8_t compression, uint32_t baud) {

	/*
	   * Begin VC0706 CMOS Camera setup
	   *
	   *  - Start with delay of 2.5s before reset, reset, then delay of 1s after to allow reset to finish
	   *  - Set image resolution, preferably low so that data transfer and image analysis is fast
	   *  - Set compressibility, preferably fast so that data transfer is quick but may cost in terms of accuracy?
	   *  - Baudrate is also variable, we could set it to 115200 baud for faster data transfer but if the camera resets, it will go back to 38400 baud
	   *
	   *  All of the above steps should really only be handled one time, at system initialization. If we are disconnected from power, however,
	   *  the camera is supposed to be reset in its normal flow. This is likely not be necessary, more testing is needed.
	   */
	  uint8_t reset_ack[5];
	  reset_ack[0] = 0;
	  uint8_t* reset_ack1 = reset_ack;

	  Camera_Reset(reset_ack1, sizeof(reset_ack));

	  if (reset_ack[0] == 0x76)
		  printf("Ack for reset received.\n");
	  else
	  	  printf("No ack received for reset.\n");

	  uint8_t resol_ack[5];
	  uint8_t* resol_ack1 = resol_ack;
	  Camera_Set_Res(resol_ack1, sizeof(resol_ack), resolution);

	  if (resol_ack[0] == 0x76)
		  printf("Ack for resolution received.\n");
	  else
		  printf("No ack received for resolution.\n");

	  uint8_t compress_ack[5];
	  uint8_t* compress_ack1 = compress_ack;
	  Camera_Set_Compress(compress_ack1, sizeof(compress_ack), compression);

	  if (compress_ack[0] == 0x76)
		  printf("Ack for compression received.\n");
	  else
		  printf("No ack received for resolution.\n");

	  uint8_t baud_ack[5];
	  uint8_t* baud_ack1 = baud_ack;
	  Camera_Set_Baud(baud_ack1, sizeof(baud_ack), baud);

	  if (baud_ack[0] == 0x76)
		  printf("Ack for baudrate received.\n");
	  else
		  printf("No ack received for baudrate.\n");

}

// function used purely to properly DeInit and then Init with a baud rate aside from our CubeMX generation
// by default, pass huart4 and UART4 for huart and name
void UART_Init_Baud(UART_HandleTypeDef* huart, USART_TypeDef* name, uint32_t baud) {

  huart->Instance = name;
  huart->Init.BaudRate = baud;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(huart) != HAL_OK)
  {
    Error_Handler();
  }

}


// dummy functions to be used as interfaces to other blocks
void get_TOF_Data(uint8_t* buf) {

}


// attempt at dealing with UART timing issues
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	//printf("Interrupt triggered.");

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
 * implement. It is extremely minimal. The variables passed are still in line with what a typical UART HAL driver might expect.
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
