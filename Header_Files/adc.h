 /*
  ******************************************************************************
  * @file    adc.h
  * @author  Denis Egorov
  * @version V1.1
  * @date    12/12/2020
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "def.h" 
#include "MDR32F9Qx_adc.h"              // Keil::Drivers:ADC
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK

/* User define ---------------------------------------------------------------*/
#define HIGH_LEVEL     1638 //4096 
#define LOW_LEVEL      1365 //3413
#define ADC_RESULT_MSK (uint32_t)0x0FFF

/* Variables -----------------------------------------------------------------*/

/* Exported functions prototypes -------------------------------------------- */
void ADC_Configuration(void); //ADC initialization function
void GetChargeLevel(uint8_t* Str_Array); //Getting battery level data

#endif 
