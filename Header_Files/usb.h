 /*
  ******************************************************************************
  * @file    usb.h
  * @author  Denis Egorov
  * @version V1.0
  * @date    05/26/2022
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_H
#define __USB_H

/* Includes ------------------------------------------------------------------*/
#include "def.h" 
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "MDR32F9Qx_usb_handlers.h"

/* User define ---------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Exported functions prototypes -------------------------------------------- */
void USB_Configuration(void); //USB initialization function 
void USB_Deinit(void); //USB deinitialization function
bool USB_Connect(void); //USB connection function (SE to PC)
bool USB_WriteCfg(struct BCData* Cfg); //Writing configuration information

#endif 
