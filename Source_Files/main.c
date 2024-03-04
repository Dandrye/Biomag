/*
  ******************************************************************************
  * @file    main.c
  * @author  Denis Egorov 
  * @version V3.0
  * @date    05/29/2022
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "def.h"
#include "init.h"
#include "menu.h"
#include "date_time.h" 
#include "meas.h"
#include "sd_card.h"
#include "dspl.h"
#include "usb.h"
#include "MDR32F9Qx_it.h"

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
PORT_InitTypeDef    PORT_InitStructure;
uint32_t rec_ptr = 0;

bool   usb_connect = FALSE;
bool   to_write = FALSE;
struct BCData config_data;
extern bool usb_req;
extern bool check_sens;
extern uint8_t menu_cnt;
extern uint8_t menu_cnt_old;
extern uint8_t menu_stat;
extern uint8_t dspl_start;
extern struct CalibData calib_factor;
extern struct BMData basic_data;
extern uint8_t  string_array[];
extern uint16_t meas_data[];
extern volatile uint8_t new_days;
extern volatile bool new_time;
extern volatile bool btn_stat;

/* Private funtion prototypes ---------------------------- */
void Delay1(uint16_t nCount);

/* MAIN FINCTION ----------------------------------------- */
int main(void)
{
    Sys_Init();	//HCLK = 48MHz  
	  NVIC_ClearPendingIRQ(EXT_INT3_IRQn);
	  NVIC_EnableIRQ(EXT_INT3_IRQn); //Enable the menu button interrupt (EXT_INT3)
	  while (btn_stat != TRUE)
		{
			 if (usb_req == TRUE)
			 {
		     if (usb_connect != TRUE)
			   {
				     usb_connect = USB_Connect();
					   /*if (usb_connect == TRUE)
						 {
                 PORT_SetBits(MDR_PORTC, PORT_Pin_13);
						 }*/
			   }
				 else					 
				 {
					   if (USB_WriteCfg(&config_data) == TRUE)
					   {  
							   SetCfgReg(&config_data);
			           SetSysFlag(START_FLAG);
			           WriteCfgReg(PTR_REG, rec_ptr);
						 } 
					   usb_connect = FALSE;						 
				 }
				 usb_req = FALSE;
       }				 
		}	
		btn_stat = FALSE;
		
		RST_Reconfiguration(); //HCLK = 8MHz
	  LoadStrArray(string_array, zero_string);
	  dspl_start = DSPL_Start(DSPL_ADDR_1);
	  DSPL_FillStr(DSPL_ADDR_1, string_array);		
		RestartTimer(MDR_TIMER2);	  
	  while (TIMER_GetFlagStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) != SET)
		{			
			  if (btn_stat == TRUE)
        {					
					  BKP_DeInit(); 
            btn_stat = FALSE;					
        }				
		}	
		while ((PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_15) == Bit_SET))
    {
		}			
    TIMER_Cmd(MDR_TIMER2,DISABLE);
	  NVIC_DisableIRQ(EXT_INT3_IRQn); //Disable the menu button interrupt (EXT_INT3)
		NVIC_ClearPendingIRQ(EXT_INT3_IRQn); 
		dspl_start = DSPL_Stop(DSPL_ADDR_1);
    	  
    if (GetSysFlagStatus(START_FLAG) != SET )
		{
		    ReadCfgFile("BM.cfg", &config_data); SetCfgReg(&config_data);
			  SetSysFlag(START_FLAG);
			  WriteCfgReg(PTR_REG, rec_ptr);
		   
		}
		else
		{
        config_data.ID = ReadCfgReg(ID_REG);
			  config_data.Date = ReadCfgReg(DATE_REG);
			  config_data.Time = ReadCfgReg(TIME_REG);
			  config_data.PolTime_T = ReadCfgReg(TPT_REG);
			  config_data.PolTime_M = ReadCfgReg(MPT_REG);
		}
		
		BKP_Init(&config_data);		
		rec_ptr = ReadCfgReg(PTR_REG);
		basic_data.ID = ReadCfgReg(ID_REG);
		
		PORT_SetBits(MDR_PORTA, PORT_Pin_7);
		PORT_SetBits(MDR_PORTB, PORT_Pin_9);
		Delay1(50);
		check_sens = CheckSensorsInterface(string_array);		
		if (check_sens == TRUE)
	  {
		    ConfigTmprSensor();
	      ReadTmprCompData(&calib_factor);
			  ConfigMagSensor();
        ReadMagAdjData(&calib_factor);			
		
			  ReadTmprData(meas_data);
				basic_data.Tmpr = GetActualTmpr(meas_data, calib_factor);
			  ReadMagData(meas_data, AVR_CFF);					
        GetMagStrength(meas_data, &basic_data);
			  basic_data.Time = ReadCfgReg(TIME_REG);
			  basic_data.Date = ReadCfgReg(DATE_REG);
				to_write = TRUE;        			
		}	
		PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
		PORT_ResetBits(MDR_PORTB, PORT_Pin_9);
		
		dspl_start = DSPL_Start(DSPL_ADDR_1);
	  DSPL_FillStr(DSPL_ADDR_1, string_array);		
	  TIMER_Cmd(MDR_TIMER2,ENABLE);
	  while (TIMER_GetFlagStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) != SET)
		{
		}		
		TIMER_Cmd(MDR_TIMER2,DISABLE);
		TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
  	NVIC_ClearPendingIRQ(EXT_INT3_IRQn);
		NVIC_EnableIRQ(EXT_INT3_IRQn);
		dspl_start = DSPL_Stop(DSPL_ADDR_1);	
		
    while(1)
    {
			  if (menu_stat == 0)
				{
            if (new_time == TRUE)
				    {
				        basic_data.Time = GetTime(string_array);	
                WriteCfgReg(TIME_REG, basic_data.Time);					
                new_time = FALSE;					
				    }
			      if (new_days != 0)
				    {
					      UpdateDate(&basic_data, new_days);
       			    WriteCfgReg(DATE_REG, basic_data.Date);		
					      new_days = 0;
				    }				
				    if (GetSysFlagStatus(TMPR_FLAG) == SET)
			      {
					      if (check_sens == TRUE)
		            {	
									  PORT_SetBits(MDR_PORTA, PORT_Pin_7);
		                PORT_SetBits(MDR_PORTB, PORT_Pin_9);
		                Delay1(50);
							      ReadTmprData(meas_data);
				            basic_data.Tmpr = GetActualTmpr(meas_data, calib_factor);	
                    PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
		                PORT_ResetBits(MDR_PORTB, PORT_Pin_9);									
						    }				  
						    ResetSysFlag(TMPR_FLAG);
					      to_write = TRUE;
			      }
			      if (GetSysFlagStatus(MAG_FLAG) == SET)    
			      { 	                 							
				        if (check_sens == TRUE)
		            {	
									  PORT_SetBits(MDR_PORTA, PORT_Pin_7);
		                PORT_SetBits(MDR_PORTB, PORT_Pin_9);
		                Delay1(50);
							      ReadMagData(meas_data, AVR_CFF);					
                    GetMagStrength(meas_data, &basic_data);	
                    PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
		                PORT_ResetBits(MDR_PORTB, PORT_Pin_9);									
						    }					
                ResetSysFlag(MAG_FLAG);
                to_write = TRUE;	
				    }
				    if (to_write == TRUE)
				    {
				        PORT_SetBits(MDR_PORTC, PORT_Pin_13);
							  WriteDataFile("BM.dat", &basic_data, &rec_ptr);	 
                WriteCfgReg(PTR_REG, rec_ptr);
					      to_write = FALSE;	
                PORT_ResetBits(MDR_PORTC, PORT_Pin_13);							
				    }							
				    if (btn_stat == TRUE)
				    {
 					      menu_stat = CheckMenuBtn(&menu_cnt);
					      btn_stat = FALSE;		
				    }
				}
			  if (menu_cnt != menu_cnt_old)
	      {  
		        if (dspl_start == 0)
						{
							  dspl_start = DSPL_Start(DSPL_ADDR_1);
						}
					  menu_cnt_old = menu_cnt; 
			      Change_Menu_Pos(menu_cnt, string_array);
					  DSPL_FillStr(DSPL_ADDR_1, string_array);
						RestartTimer(MDR_TIMER2);
           	if (menu_stat == 0)
            {		
                NVIC_ClearPendingIRQ(EXT_INT3_IRQn);							
                NVIC_EnableIRQ(EXT_INT3_IRQn);
						}							
	      }
				else
				{
				    if ((menu_stat == 1) && (PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_15) != Bit_SET))
            {
                menu_stat = 0; 
                NVIC_ClearPendingIRQ(EXT_INT3_IRQn);
				        NVIC_EnableIRQ(EXT_INT3_IRQn);
				    }	
				}						
        if (TIMER_GetFlagStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) == SET)
        {
            TIMER_Cmd(MDR_TIMER2,DISABLE);
					  if ((menu_stat == 1) && (PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_15) == Bit_SET))
						{
						     menu_cnt++;
                 if (menu_cnt > 10)
	               {
                     menu_cnt = 1;  
	               }	
					  }
						else
						{                  							
							  menu_cnt = 0;
					      menu_cnt_old = 0;
							  menu_stat = 0;
							  dspl_start = DSPL_Stop(DSPL_ADDR_1);	
							 /*if (check_sens == TRUE)
								{
									 PORT_SetBits(MDR_PORTA, PORT_Pin_7);
		               PORT_SetBits(MDR_PORTB, PORT_Pin_9);
									 Delay1(50);
							     ReadMagData(meas_data, AVR_CFF);					
                   GetMagStrength(meas_data, &basic_data);
							     ReadTmprData(meas_data);
				           basic_data.Tmpr = GetActualTmpr(meas_data, calib_factor);
									 PORT_ResetBits(MDR_PORTA, PORT_Pin_7);		               
									 PORT_ResetBits(MDR_PORTB, PORT_Pin_9);	
								}*/
						}		        				 
			  }
				
				if ((menu_stat == 0) && (dspl_start == 0))
        {				  
            __WFI();	
		    }	
    }						
}

/* Private funtion --------------------------------------- */

void Delay1(uint16_t nCount)
{
   uint32_t temp;
  
   temp = 80000*nCount;	
	 for (; temp != 0; temp--);
}

#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */


