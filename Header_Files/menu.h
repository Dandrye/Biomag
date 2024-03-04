 /*
  ******************************************************************************
  * @file    menu.h
  * @author  Denis Egorov
  * @version V1.7
  * @date 05/13/2021   
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_timer.h"            // Keil::Drivers:TIMER

/* User define ---------------------------------------------------------------*/
#define SIGN_MSK   0x8000
#define CHAR_MSK   0x000F

/* Variables -----------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
uint8_t CheckMenuBtn(uint8_t* Cnt); //Checking status of menu button
void Change_Menu_Pos(uint8_t Cnt, uint8_t* Str_Array); //Displaying menu position
void RestartTimer(MDR_TIMER_TypeDef* TIMERx); //Restarting timer

/* Functions of interrupts handling*/ 

#endif 
