/*
  ******************************************************************************
  * @file    date_time.h
  * @author  Denis Egorov
  * @version V1.4
  * @date    12/11/2020 
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATE_TIME_H
#define __DATE_TIME_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "MDR32F9Qx_bkp.h"              // Keil::Drivers:BKP

/* User define ---------------------------------------------------------------*/

/* Exported Variables --------------------------------------------------------*/

/* Exported functions prototypes -------------------------------------------- */
uint8_t GetNewDays(void); //Checking if new days starts
uint16_t GetTime(uint8_t* Str_Array); //Getting actual time 
void UpdateDate(struct BMData* Date, uint8_t NewDays); //Updating current date
void GetDate(struct BMData* Date, uint8_t* Str_Array); //Getting actual date
void WriteCfgReg(cfg_reg Reg, uint32_t Value); //Writing data to the BKP register
uint32_t ReadCfgReg(cfg_reg Reg); //Reading data from the BKP register
void SetCfgReg(struct BCData* CfgData); //Setting data the BKP registers
void GetCfgReg(struct BCData* CfgData); //Getting data from the BKP registers
void UpdateTimeCnt(void); //Time counters update
FlagStatus GetSysFlagStatus(sys_flag Flag); //Getting the status of an action flag
void SetSysFlag(sys_flag Flag); //Set the action flag 
void ResetSysFlag(sys_flag Flag); //Reset the action flag
void SetPollCounter(cnt_reg Reg, uint32_t Value); //Writing the poll-counter register
void ClearSysFlags(void); //Clearing the action flags
void ClearPollCounters(void); //Clearing the poll-counter register

#endif 
