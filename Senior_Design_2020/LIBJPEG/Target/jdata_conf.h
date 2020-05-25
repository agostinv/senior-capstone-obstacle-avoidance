/**
 ******************************************************************************
  * File Name          : jdata_conf.h
  * Description        : This file provides header to "jdata_conf.h" module.
  *                      It implements also file based read/write functions.
  *
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

/* Includes ------------------------------------------------------------------*/

/*Stdio is chosen for File storage*/
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*This defines the memory allocation methods.*/
#define JMALLOC   malloc
#define JFREE     free

/*This defines the File data manager type.*/
#define JFILE            FILE

#define JFREAD(file,buf,sizeofbuf)  \
((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

#define JFWRITE(file,buf,sizeofbuf)  \
((size_t) fwrite((const void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
