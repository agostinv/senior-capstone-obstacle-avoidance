/*
 * VC0706_comm.c
 *
 *  Created on: May 24, 2020
 *      Author: victo
 */

#include "VC0706_comm.h"


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

// after reading, you must stop capture before continuing
uint8_t CAM_STOP_CAP[]						=	{0x56, 0x0, 0x36, 0x01, 0x03};
uint8_t CAM_STOP_CAP_ACK[]					=	{0x76, 0x0, 0x36, 0x0, 0x0};


// necessary global variables needed
uint8_t junk[255];
uint8_t img_buf[67200];


// required set-up process for the camera to function, according to documentation
void Camera_Reset(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec)
{
	printf("Size of receiving buffer: %d\n", len_rec);
	HAL_Delay(2500);	//2500 ms delay
	send_UART(huart, CAM_RESET, rec, sizeof(CAM_RESET), len_rec);

	for (int i = 0; i < len_rec; i++) {
		printf("%d ", rec[i]);
	}
	printf("\n");

	HAL_Delay(1000);
}

// abstracted function for general USART exchange, note that size of rec must be known beforehand for this to work
bool send_UART(UART_HandleTypeDef* huart, uint8_t* comm, uint8_t* rec, uint8_t len_comm, uint16_t len_rec)
{
	HAL_StatusTypeDef res = HAL_ERROR;
	res = HAL_UART_Receive(huart, junk, 255, 2);
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
	      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tick3, 100) != HAL_OK)
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
	       if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, 3000) != HAL_OK)
	       {
	    	 printf("\n\nBROKE LOOP ON RX, TIMEOUT AT %d ITERATION\n\n", len_rec - huart->RxXferCount);
	         return HAL_TIMEOUT;
	       }

	       *pData = (uint8_t)(huart->Instance->RDR & (uint8_t) huart->Mask);	// note that UART mask here is variable in HAL drivers, often 255 so that is default
	       pData++;

	       huart->RxXferCount--;

	    }

	res = HAL_UART_Receive(huart, junk, 255, 2);
}

//orders camera to take snapshot
void Camera_Snapshot(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec) {
	send_UART(huart, CAM_CAPTURE, rec, sizeof(CAM_CAPTURE), len_rec);

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");

	HAL_Delay(1);
}

void Camera_Stop_Capture(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec) {
	send_UART(huart, CAM_STOP_CAP, rec, sizeof(CAM_STOP_CAP), len_rec);

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");

	HAL_Delay(1);
}

//returns 2-value array of characters, first is high part of 16-bit length, second is low part
void Camera_Get_IMG_Length(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec) {
	send_UART(huart, CAM_IMG_LENGTH, rec, sizeof(CAM_IMG_LENGTH), len_rec);

	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");
}

//records the image into a statically allocated array of the given
void Camera_Get_IMG(UART_HandleTypeDef* huart, uint8_t* rec, uint16_t len_rec, uint8_t lengthH, uint8_t lengthL, uint8_t memH, uint8_t memL) {

	CAM_READ_IMG[12] = lengthH;
	CAM_READ_IMG[13] = lengthL;

	CAM_READ_IMG[8] = memH;
	CAM_READ_IMG[9] = memL;

	send_UART(huart, CAM_READ_IMG, rec, sizeof(CAM_READ_IMG), len_rec);

	/*printf("IMAGE:\n\n");
	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");*/

}

//sets the resolution of the image, full resolution likely not needed for crude processing
//0x00 is default for 640x480, to get 320x240 must manually send command, replace 0x00 with 0x11
// resolution = 6 sends 640x480, resolution = 3 sends 320x240, resolution = 1 sends 160x120
void Camera_Set_Res(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec, uint8_t res) {

	// for 640x480
	if (res == 6)
		send_UART(huart, CAM_RES_640_480, rec, sizeof(CAM_RES_640_480), len_rec);

	// for 320x240
	else if (res == 3)
		send_UART(huart, CAM_RES_320_240, rec, sizeof(CAM_RES_320_240), len_rec);

	// for 160x120
	else if (res == 1)
		send_UART(huart, CAM_RES_160_120, rec, sizeof(CAM_RES_160_120), len_rec);

	for (int i = 0; i < len_rec; i++)
			printf("%d ", rec[i]);
		printf("\n");
}

// function to ease customization of compression, higher compression is vastly preferable
void Camera_Set_Compress(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec, uint8_t comp) {
	CAM_COMPRESS[8] = comp;

	send_UART(huart, CAM_COMPRESS, rec, sizeof(CAM_COMPRESS), len_rec);

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");
}

// function to set the baud rate, pass the baud rate directly for check
void Camera_Set_Baud(UART_HandleTypeDef* huart, uint8_t* rec, uint8_t len_rec, uint32_t baud) {

	if (baud == 9600) {
		send_UART(huart, CAM_BAUD_9600, rec, sizeof(CAM_BAUD_9600), len_rec);
		HAL_UART_DeInit(huart);
		HAL_Delay(1);
		UART_Init_Baud(huart, UART8, 9600);
	}

	else if (baud == 19200) {
		send_UART(huart, CAM_BAUD_19200, rec, sizeof(CAM_BAUD_19200), len_rec);
		HAL_UART_DeInit(huart);
		HAL_Delay(1);
		UART_Init_Baud(huart, UART8, 19200);
	}

	else if (baud == 38400) {
		send_UART(huart, CAM_BAUD_38400, rec, sizeof(CAM_BAUD_38400), len_rec);
		HAL_UART_DeInit(huart);
		HAL_Delay(1);
		UART_Init_Baud(huart, UART8, 38400);
	}

	else if (baud == 57600) {
		send_UART(huart, CAM_BAUD_57600, rec, sizeof(CAM_BAUD_57600), len_rec);
		HAL_UART_DeInit(huart);
		HAL_Delay(1);
		UART_Init_Baud(huart, UART8, 57600);
	}

	else if (baud == 115200) {
		send_UART(huart, CAM_BAUD_115200, rec, sizeof(CAM_BAUD_115200), len_rec);
		HAL_UART_DeInit(huart);
		HAL_Delay(1);
		UART_Init_Baud(huart, UART8, 115200);
	}

	for (int i = 0; i < len_rec; i++)
		printf("%d ", rec[i]);
	printf("\n");
}

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
void Camera_Initialize(UART_HandleTypeDef* huart, uint8_t resolution, uint8_t compression, uint32_t baud) {
	  uint8_t reset_ack[5];
	  reset_ack[0] = 0;
	  uint8_t* reset_ack1 = reset_ack;

	  Camera_Reset(huart, reset_ack1, sizeof(reset_ack));

	  if (reset_ack[0] == 0x76)
		  printf("Ack for reset received.\n");
	  else
	  	  printf("No ack received for reset.\n");

	  uint8_t resol_ack[5];
	  uint8_t* resol_ack1 = resol_ack;
	  Camera_Set_Res(huart, resol_ack1, sizeof(resol_ack), resolution);

	  if (resol_ack[0] == 0x76)
		  printf("Ack for resolution received.\n");
	  else
		  printf("No ack received for resolution.\n");

	  uint8_t compress_ack[5];
	  uint8_t* compress_ack1 = compress_ack;
	  Camera_Set_Compress(huart, compress_ack1, sizeof(compress_ack), compression);

	  if (compress_ack[0] == 0x76)
		  printf("Ack for compression received.\n");
	  else
		  printf("No ack received for resolution.\n");

	  uint8_t baud_ack[5];
	  uint8_t* baud_ack1 = baud_ack;
	  Camera_Set_Baud(huart, baud_ack1, sizeof(baud_ack), baud);

	  if (baud_ack[0] == 0x76)
		  printf("Ack for baudrate received.\n");
	  else
		  printf("No ack received for baudrate.\n");

}

// function used purely to properly DeInit and then Init with a baud rate aside from our CubeMX generation
// by default, pass huart8 and UART8 for huart and name
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
  huart->AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_ENABLE;
  huart->AdvancedInit.AutoBaudRateMode = UART_ADVFEATURE_AUTOBAUDRATE_ONSTARTBIT;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    Cam_Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Cam_Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
  {
    Cam_Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(huart) != HAL_OK)
  {
    Cam_Error_Handler();
  }

}

// currently useless, but could be filled if desired
void Cam_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

