 /*
  ******************************************************************************
  * @file    init.h
  * @author  Denis Egorov
  * @version V1.3 
  * @date    05/26/2022
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_H
#define __INIT_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "MDR32F9Qx_eeprom.h"           // Keil::Drivers:EEMPROM
#include "MDR32F9Qx_bkp.h"              // Keil::Drivers:BKP
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_i2c.h"              // Keil::Drivers:I2C
#include "MDR32F9Qx_timer.h"            // Keil::Drivers:TIMER

/* User define ---------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Sys_Init(void); //Start Initialization
void RST_Reconfiguration(void); //RST reconfiguration function
void Pin_Configuration(void); //PIN configuration function
void TIM_Configuration(void); //TIM configuration function
void BKP_Init(struct BCData* CntVal); //BKP Initialization

/* Functions of interrupts handling */ 

#endif 
