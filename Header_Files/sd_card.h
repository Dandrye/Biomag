 /*
  ******************************************************************************
  * @file    sd_card.h
  * @author  Denis Egorov
  * @version V1.2
  * @date    05/06/2021
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_CARD_H
#define __SD_CARD_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"

/* User define ---------------------------------------------------------------*/
typedef enum
{
    OK = 0, ERR  = !OK
} RW_RESULT;

#define DATA_FILE_SIZE 2000000000

/* Exported macro ------------------------------------------------------------*/
void ReadCfgFile(const char* FileName, struct BCData* Cfg); //Reading data from the SD Card CFG file 
void WriteDataFile(const char* FileName, struct BMData* Data, uint32_t* FPtr); //Writing data to the SD Card DAT file  

#endif 
