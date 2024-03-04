/*
  ******************************************************************************
  * @file    date_time.c
  * @author  Denis Egorov
  * @version V2.4
  * @date    01/23/2022  
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "date_time.h"

/* Private typedif --------------------------------------- */
 
/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */

/* Private funtion prototypes ---------------------------- */
uint8_t IsLeapYear(uint16_t nYear); //Checking whether the passed year is Leap or not
uint32_t GetPollCounter(cnt_reg Reg); //Reading the poll-counter register

/* Interrupts handling ----------------------------------- */

/* Exported functions ------------------------------------ */
uint8_t GetNewDays(void)
{
	  uint32_t temp1, temp2;
	
	  BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
	  temp1 = BKP_RTC_GetCounter();
	  BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
    temp2 = BKP_RTC_GetCounter();
	  if (temp1 != temp2)
    { 
			  BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
		    temp1 =	BKP_RTC_GetCounter();
		}			
	  if (temp1 >= 86399) //If counter is equal or more then 86399: one or more days were elapsed
    {
        /* Count how many days have passed */
		    temp1 += 1;
			  temp2 = temp1;
			  temp2 /= 86399; 
		    temp1 %= 86399;	
        BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished			
        BKP_RTC_SetCounter(temp1); //Set new counter value
			  temp1 += ALARM_VALUE;
			  BKP_RTC_WaitForUpdate();
        BKP_RTC_SetAlarm(temp1);
			
        return temp2;
	  }
    else
	  {
        return 0;
	  }			
}

uint16_t GetTime(uint8_t Str_Array[])
{
    uint16_t result;
	  uint32_t temp, temp1;
	
	  BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
	  temp = BKP_RTC_GetCounter();
    BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
    temp1 = BKP_RTC_GetCounter();
	  if (temp != temp1)
    { 
			  BKP_RTC_WaitForUpdate(); //Wait until last operation on RTC registers has finished	
			  temp =	BKP_RTC_GetCounter();
		}				
	  temp %= 86399;
	  result = temp/3600;
	  result = result << 8;
	  Str_Array[3] = temp/36000;
	  temp %= 36000;
	  Str_Array[4] = temp/3600; 
	  temp %= 3600;
	  result |= temp/60;
    Str_Array[6] = temp/600;
    temp %= 600;	
	  Str_Array[7] = temp/60;
	  return result;
}

void UpdateDate(struct BMData* Date, uint8_t NewDays)
{
    uint8_t  month, day; //temp;
	  uint16_t year;
	  uint32_t in_date, out_date;
		
	  in_date = Date->Date;	
	  year = (uint8_t)in_date;
	  month = ((uint8_t)(in_date >> 8));
	  day = ((uint8_t)(in_date >> 16) + NewDays); 
   
	  if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
        if (day <= 31)
        {
            out_date = day;
					  out_date = (out_date << 8) +  month;
					  out_date = (out_date << 8) +  year;
        }
        else
        {
            if (month != 12)
            { 
					      out_date = (day - 31);
							  month++;
					      out_date = (out_date << 8) +  month;
                out_date = (out_date << 8) +  year;							
            }
            else
            {
                out_date = (day - 31);
					      year++;
					      out_date = (out_date << 8) + 1;
							  out_date = (out_date << 8) +  year;
            }
        }
		}
    else
    {		
        if (month == 4 || month == 6 || month == 9 || month == 11)
        {
            if (day <= 30)
            {
					      out_date = day;
					      out_date = (out_date << 8) +  month;
					      out_date = (out_date << 8) +  year;
            }         
            else
            {
                out_date = (day - 30);
							  month++;
					      out_date = (out_date << 8) +  month;
                out_date = (out_date << 8) +  year;	
            }
		    }
        else 
		    {
				    if (day < 28)
            {
                out_date = day;
								out_date = (out_date << 8) +  month;
					      out_date = (out_date << 8) +  year;
            }
            else
					  {						 
						    if (day == 28)
                {               
                    if (IsLeapYear(year) == TRUE)
                    {
                        out_date = day;
									      out_date = (out_date << 8) +  month;
					              out_date = (out_date << 8) +  year;
                    }
                    else
                    {
										    out_date = (day - 28);
							          month++;
					              out_date = (out_date << 8) +  month;
                        out_date = (out_date << 8) +  year;	                    
                    }
							  }
								else 
				        {
					          out_date = (day - 29);
									  out_date = (out_date << 8) +  month;
					          out_date = (out_date << 8) +  year;               
                }
            }  								
			  }  
    }
		Date->Date = out_date;		
}

void GetDate(struct BMData* Date, uint8_t Str_Array[])
{
    uint8_t  temp;
	  uint32_t temp1;
	  
	  temp1 = Date->Date;
    temp = (uint8_t)temp1;
	  Str_Array[8] = temp/10;
	  Str_Array[9] = temp%10;
	  temp = (uint8_t)(temp1 >> 8);
	  Str_Array[3] = temp/10;
	  Str_Array[4] = temp%10;
	  temp = (uint8_t)(temp1 >> 16);
	  Str_Array[0] = temp/10;
	  Str_Array[1] = temp%10;
}	

void UpdateTimeCnt(void)
{
	  uint32_t temp, temp1;
	  
	  temp = ReadCfgReg(TPT_REG);
	  temp1 = GetPollCounter(TCNT_REG);
	  temp1++;
	  if (temp1 >= temp)
		{
			  SetSysFlag(TMPR_FLAG);
			  SetPollCounter(TCNT_REG, 0);		
		}
		else
		{
			  SetPollCounter(TCNT_REG, temp1);
		}
    temp = ReadCfgReg(MPT_REG);
	  temp1 = GetPollCounter(MCNT_REG);
	  temp1++;
	  if (temp1 >= temp)
		{   
			  SetSysFlag(MAG_FLAG);			  
        SetPollCounter(MCNT_REG, 0);		
		}
    else
		{
        SetPollCounter(MCNT_REG, temp1);
		}			
}

void WriteCfgReg(cfg_reg Reg, uint32_t Value)
{
    BKP_RTC_WaitForUpdate();
	  switch (Reg)
	  {
			  case ID_REG:
				{ 	
            MDR_BKP->REG_04 = Value;
						break;
				}					
		    case DATE_REG:  
				{ 	
            MDR_BKP->REG_05 = Value;
						break;
				}	
				case TIME_REG:  
				{ 	
            MDR_BKP->REG_06 = Value;
						break;
				}
        case TPT_REG:  
				{ 	
            MDR_BKP->REG_07 = Value;
						break;
				}
        case MPT_REG:  
				{ 	
            MDR_BKP->REG_08 = Value;
						break;
				}
				case PTR_REG:  
				{ 	
            MDR_BKP->REG_09 = Value;
						break;
				}
		}
}

uint32_t ReadCfgReg(cfg_reg Reg)
{
    uint32_t temp;	
	
	  BKP_RTC_WaitForUpdate();
	  switch (Reg)
	  {
			  case ID_REG:  
				{ 	
            temp = MDR_BKP->REG_04;
						break;
				}	
		    case DATE_REG:  
				{ 	
            temp = MDR_BKP->REG_05;
						break;
				}	
				case TIME_REG:  
				{ 	
            temp = MDR_BKP->REG_06;
						break;
				}
        case TPT_REG:  
				{ 	
            temp = MDR_BKP->REG_07;
						break;
				}
        case MPT_REG:  
				{ 	
            temp = MDR_BKP->REG_08;
						break;
				}
        case PTR_REG:  
				{ 	
            temp = MDR_BKP->REG_09;
						break;
				}				
		}				
		return temp;
}

void SetCfgReg(struct BCData* CfgData)
{
    uint32_t temp;
	
	  temp = CfgData->ID;
	  WriteCfgReg(ID_REG, temp);
	  temp = CfgData->Date;
	  WriteCfgReg(DATE_REG, temp);
    temp = CfgData->Time;
    WriteCfgReg(TIME_REG, temp);	
	  temp = (uint32_t)(CfgData->PolTime_T);
	  WriteCfgReg(TPT_REG, temp);
	  temp = (uint32_t)(CfgData->PolTime_M);
	  WriteCfgReg(MPT_REG, temp);
	  ClearSysFlags();
	  ClearPollCounters();		
}

void GetCfgReg(struct BCData* CfgData)	
{   
	  CfgData->ID = (uint8_t)ReadCfgReg(ID_REG);
	  CfgData->Date = ReadCfgReg(DATE_REG);
    CfgData->Time = (uint16_t)ReadCfgReg(TIME_REG);
    CfgData->PolTime_T = (uint16_t)ReadCfgReg(TPT_REG); 	
	  CfgData->PolTime_M = (uint16_t)ReadCfgReg(MPT_REG); 
}

void SetSysFlag(sys_flag Flag)
{
	  BKP_RTC_WaitForUpdate();
    switch (Flag)
    { 
			  case START_FLAG:
			  {
				    MDR_BKP->REG_0D = FIRST_START_MSK;
				    break;
			  }
			  case TMPR_FLAG:
			  {
				    MDR_BKP->REG_0C |= TMPR_FLAG_MSK;
				    break;
			  }
			  case MAG_FLAG:
			  {
				    MDR_BKP->REG_0C |= MAG_FLAG_MSK;
				    break;
			  }			
		}	 	
}

void ResetSysFlag(sys_flag Flag)
{
    BKP_RTC_WaitForUpdate();
    switch (Flag)
    { 
			  case START_FLAG:
			  {
				    MDR_BKP->REG_0D = 0;
				    break;
			  }
			  case TMPR_FLAG:
			  {
				    MDR_BKP->REG_0C &= ~TMPR_FLAG_MSK;
				    break;
			  }
			  case MAG_FLAG:
			  {
				    MDR_BKP->REG_0C &= ~MAG_FLAG_MSK;
				    break;
			  }			
		}	 	    	
} 

void ClearSysFlags(void)
{
     BKP_RTC_WaitForUpdate();
     MDR_BKP->REG_0C = 0;
	   BKP_RTC_WaitForUpdate();
     MDR_BKP->REG_0D = 0;
}

FlagStatus GetSysFlagStatus(sys_flag Flag)
{
    uint32_t temp;
	
	  BKP_RTC_WaitForUpdate();
	  temp = MDR_BKP->REG_0C; 
	  switch (Flag)
    { 
			  case START_FLAG:
			  {
					  BKP_RTC_WaitForUpdate();
					  temp = MDR_BKP->REG_0D; 
				    if (temp == FIRST_START_MSK)
            {		
							  temp = 1;
						}							
						else
						{
							  temp = 0;
						}
				    break;
			  }
			  case TMPR_FLAG:
			  {
				    temp &= TMPR_FLAG_MSK;
				    break;
			  }
			  case MAG_FLAG:
			  {
				    temp &= MAG_FLAG_MSK;
				    break;
			  }
				default:
				{
					  temp = 0;
				}
		}		
		if (temp != 0)
    {
        return SET;
		}
    else 
		{
        return RESET;
		}
}

void SetPollCounter(cnt_reg Reg, uint32_t Value)
{
	  BKP_RTC_WaitForUpdate();
	  switch (Reg)
    { 
			  case TCNT_REG:
			  {
				    MDR_BKP->REG_0A = Value;
				    break;
			  }
			  case MCNT_REG:
			  {
				    MDR_BKP->REG_0B = Value;;
				    break;
			  }			
		}	
}

void ClearPollCounters(void)
{
    BKP_RTC_WaitForUpdate();
	  MDR_BKP->REG_0A = 0;
	  BKP_RTC_WaitForUpdate();
	  MDR_BKP->REG_0B = 0; 
}	

uint32_t GetPollCounter(cnt_reg Reg)
{
    uint32_t temp;
	  
	  BKP_RTC_WaitForUpdate(); 
	  switch (Reg)
    { 
			  case TCNT_REG:
			  {
				    temp = MDR_BKP->REG_0A;
				    break;
			  }
			  case MCNT_REG:
			  {
				    temp = MDR_BKP->REG_0B;
				    break;
			  }			
		}
    return temp;		
}

/* Private functions ------------------------------------- */
uint8_t IsLeapYear(uint16_t nYear)
{
   if(nYear % 4 != 0) return 0;
   if(nYear % 100 != 0) return 1;
   return (uint8_t)(nYear % 400 == 0);
}


