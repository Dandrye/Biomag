/*
  ******************************************************************************
  * @file    menu.c
  * @author  Denis Egorov
  * @version V2.05
  * @date 02/17/2022    
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "menu.h"
#include "meas.h"
#include "adc.h"
#include "date_time.h"

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
bool check_sens = FALSE;
uint8_t menu_cnt = 0;
uint8_t menu_cnt_old = 0;
uint8_t menu_stat = 0;
uint8_t string_array[11] = {0};
extern uint16_t meas_data[];
extern struct BMData basic_data;
extern struct CalibData calib_factor;
 
/* Private funtion prototypes ---------------------------- */
void Conv_To_Str(struct BMData* Data, uint8_t* Array, uint8_t Mode); //Converting measured data to a display string
void Delay(uint16_t nTime); //Adjustable delay time function

/* Exported functions ------------------------------------ */
uint8_t CheckMenuBtn(uint8_t* Cnt)
{  
  uint8_t temp;
	uint8_t result;
	
  temp = *Cnt;
	result = 0;
	Delay(50);
	RestartTimer(MDR_TIMER3);
	while ((PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_15) == Bit_SET)) 		    
  {						  
      Delay(10);
      if (TIMER_GetFlagStatus(MDR_TIMER3, TIMER_STATUS_CNT_ARR) == SET)
      {
           result = 1;				
           break;				
      }				
	}
	NVIC_ClearPendingIRQ(EXT_INT3_IRQn);
	TIMER_Cmd(MDR_TIMER2,DISABLE);
  TIMER_Cmd(MDR_TIMER3,DISABLE);	
  temp++;
  if (temp > 10)
	{
      temp = 1;  
	}
	*Cnt = temp;	
  return result;	
}

void Change_Menu_Pos(uint8_t Cnt, uint8_t* Str_Array)
{
	  uint8_t Idx;
	
    switch (Cnt)
		{
		    case 0:  
				{ 	
            for (Idx = 0; Idx < 11; ++Idx) 
			      {   
							  Str_Array[Idx] = start_string[Idx];
						}
						break;
				}
				case 1: 
        { 
					  PORT_SetBits(MDR_PORTB, PORT_Pin_5);
					  Delay(10);
					  for (Idx = 0; Idx < 11; ++Idx) 
			      {   
						    Str_Array[Idx] = charge_string[Idx];							  
						}
						if (menu_stat == 0)
						{
						    GetChargeLevel(Str_Array);							  
						}
						PORT_ResetBits(MDR_PORTB, PORT_Pin_5);
            break;
				}
			  case 2: 
				{
			    	for (Idx = 0; Idx < 11; ++Idx) 
			      {   
					      Str_Array[Idx] = hx_string[Idx];
						}
					  Conv_To_Str(&basic_data, Str_Array, MAG_X_MODE);
            break;
				}			
				case 3: 
        {
			    	for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = hy_string[Idx];
						}
						Conv_To_Str(&basic_data, Str_Array, MAG_Y_MODE);
            break;
				}			
				case 4: 
        {
    				for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = hz_string[Idx];
					  }
						Conv_To_Str(&basic_data, Str_Array, MAG_Z_MODE);
            break;
				}						
				case 5:  
				{	
				    for (Idx = 0; Idx < 11; ++Idx) 
			      {  
				        Str_Array[Idx] = hx_var_string[Idx];
						}	
					 	Conv_To_Str(&basic_data, Str_Array, MAG_X_VAR_MODE);						
            break;
		 	  }				 
				case 6: 
        {
				    for (Idx = 0; Idx < 11; ++Idx) 
			      {      
					      Str_Array[Idx] = hy_var_string[Idx];							  
						}
						Conv_To_Str(&basic_data, Str_Array, MAG_Y_VAR_MODE);
            break;
				}
				case 7: 
				{
				    for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = hz_var_string[Idx];							  
					  }
						Conv_To_Str(&basic_data, Str_Array, MAG_Z_VAR_MODE);
            break;						
				}			
				case 8:  
			  {
		        for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = tmpr_string[Idx];
					  }
            Conv_To_Str(&basic_data, Str_Array, TMPR_MODE);
					  break;
			  }                 
				case 9:  
			  {  
					  for (Idx = 0; Idx < 11; ++Idx) 
            {					
					      Str_Array[Idx] = time_string[Idx];							  
						}
            GetTime(Str_Array);						
            break;
				}			
				case 10: 
				{	
				    for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = date_string[Idx];						  
						}
						GetDate(&basic_data, Str_Array);
            break;
				}						
				default: 
				{	
			   	  for (Idx = 0; Idx < 11; ++Idx) 
			      {  
					      Str_Array[Idx] = start_string[Idx];
					  }									 
        } 
		}				
}

void RestartTimer(MDR_TIMER_TypeDef* TIMERx)
{
    TIMER_Cmd(TIMERx,DISABLE);						
    TIMER_ClearFlag(TIMERx, TIMER_STATUS_CNT_ARR);
    TIMERx->CNT = 0x0001;
		TIMER_Cmd(TIMERx,ENABLE);
}	

/* Private functions ------------------------------------- */
void Conv_To_Str(struct BMData* Data, uint8_t* Array, uint8_t Mode)
{
    uint8_t Idx = 1;
	  int16_t temp1, temp2;
	
	  switch (Mode)
		{
			  case 0:
        {
					  Idx = 0; 
            temp1 = Data->Tmpr;
					  break;					
			  }
			  case 1:
        {
            temp1 = Data->Hx;					 
					  break;					
			  }
        case 2:
        {
            temp1 = Data->Hy;
					  break;					
			  } 
			  case 3:
        {
            temp1 = Data->Hz;
					  break;					
			  }
        case 4:
        {
            temp1 = Data->Hx_var;
					  break;					
			  }
				case 5:
        {
            temp1 = Data->Hy_var;
					  break;					
			  }
				case 6:
        {
            temp1 = Data->Hz_var;
					  break;					
			  }				
		}
		temp2 = temp1;
    if ((Mode == 4) || (Mode == 5) || (Mode	== 6))	
		{
			  Idx += 2;
        Array[Idx] = 0x15; //"~"		   
		}
    else
		{
			  //temp2 = temp1;
	      temp1 &= SIGN_MSK; 
		    Idx += 2;
	      if (temp1 != 0)
		    {
		        Array[Idx] = 0x13; //"-" 
            temp2 = ~temp2;
			      temp2 += 1;			
		    }
		    else
		    {
			      Array[Idx] = 0x12; //"+"
		    }      
		}
    if (temp2 > 9990)
		{
			  temp2 = 9990;
		}		
    Idx++; 
		Array[Idx] = (uint8_t)(temp2/1000);
		temp1 = temp2%1000;
		Idx++;
		Array[Idx] = (uint8_t)(temp1/100);
		temp2 =  temp1%100;		
		Idx++;
		Array[Idx] = 0x16;
		Idx++;
    Array[Idx] = (uint8_t)(temp2/10);		
}

void Delay(uint16_t nTime)
{
	  uint16_t  delay1, delay2;
	  		
	  delay2 = nTime;
	  while (delay2 != 0)
	  {
		   delay1 = 8000;
	     while (delay1 != 0)
       {
	        delay1--;			 
	     }
       delay2--;			 
    }			 
}


