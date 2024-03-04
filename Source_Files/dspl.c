/*
  ******************************************************************************
  * @file    dspl.c
  * @author  Denis Egorov
  * @version V1.14
  * @date  04/26/2021  
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "dspl.h"

/* Private typedif --------------------------------------- */ 

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
static uint8_t init_array[23] = {0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40,
                                 0x8D, 0x14, 0xA1, 0xC8, 0xDA, 0x02, 0x81, 0xCF, 
                                 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF}; //Initialization command set										 
static uint8_t clear_array[2] = {0x20, 0x00}; //Initialization set of clearing
static uint8_t fill_array[8] = {0x20, 0x00, 0x21, 0x03, 0x7B, 0x22, 0x01, 0x02}; //Initialization set of filling 
uint8_t dspl_start = 0;

/* Private funtion prototypes ---------------------------- */
void DSPL_Config(uint8_t Addr); //Display configuration
void Delay_ms(uint16_t nTime); //Adjustable delay time function

/* Interrupts handling ----------------------------------- */

/* Exported functions ------------------------------------ */
uint8_t DSPL_Start(uint8_t Addr)
{
	  DSPL_Config(Addr);
	  DSPL_Clear(Addr);
	  return 1;
}

void DSPL_Config(uint8_t Addr)
{
	  uint8_t idx;
	
	  PORT_SetBits(MDR_PORTC, PORT_Pin_6); //Display power on
	  Delay_ms(100); //100
	
	  while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) //Checking if I2C bus is free
    {
    }
	  I2C_Send7bitAddress(Addr, I2C_Direction_Transmitter); //Sending display address
	  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
    {
    }
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
    { 
			  for (idx = 0; idx < 23; idx++) //Sending the inizialization commands
        {
            I2C_SendByte(0x80);
					  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
            {
            }
		        if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET)
            {
							  I2C_SendByte(init_array[idx]);
		        		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                {
                }		
								while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
                {
						    }	
						}								
		    }	
		}
		I2C_SendSTOP(); //Stop I2C bus	
}

void DSPL_Clear(uint8_t Addr)
{
	  uint8_t idx, col_idx, row_idx;
	
	  while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) //Checking if I2C bus is free
    {
    }
	  I2C_Send7bitAddress(Addr, I2C_Direction_Transmitter); //Sending display address
	  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
    {
    }
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
		{ 
			  //Setting the Horizontal Addressing Mode
				for (idx = 0; idx < 2; idx++)
        {
            I2C_SendByte(0x80);
					  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
            {
            }
		        if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET)
            {
							  I2C_SendByte(clear_array[idx]);
		        		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                {
                }		
								while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
                {
						    }	
						}								
		    }	
				//Sending the screen clearing data
				I2C_SendByte(0x40);
		    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
        {
				}
				if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
				{
				    for (row_idx = 0; row_idx < 4; row_idx++)
					  {
                for (col_idx = 0; col_idx < 128; col_idx++)
                {
                    I2C_SendByte(0x00);
		                while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                    {
				            }
										while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
										{
										}
								}
						}							
				}					
		}	
		I2C_SendSTOP(); //Stop I2C bus	 
}

void DSPL_FillStr(uint8_t Addr, uint8_t* Str_Array)
{   
	  uint8_t idx, ch_idx, col_idx;
	
	  while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) //Checking if I2C bus is free
    {
    }
	  I2C_Send7bitAddress(Addr, I2C_Direction_Transmitter); //Sending display address
	  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
    {
    }
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
		{
			  for (idx = 0; idx < 8; idx++) //Sending the inizialization commands
        {
            I2C_SendByte(0x80);
					  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
            {
            }
		        if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET)
            {
							  I2C_SendByte(fill_array[idx]);
		        		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                {
                }		
								while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
                {
						    }	
						}								
		    }	
				//Filling string
				I2C_SendByte(0x40);
		    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
        {
				}
				if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
		    {
				    for (idx = 0; idx < 11; idx++) //Filling first row
				    {
					      ch_idx = Str_Array[idx];
							  for (col_idx = 0; col_idx < 11; col_idx++)
					      {
						        I2C_SendByte(char_array[ch_idx][col_idx]);
						        while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                    {
					          }
								    while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
								    {
								    }
					      }
				    }
				    for (idx = 0; idx < 11; idx++) //Filling second row
				    {
							  ch_idx = Str_Array[idx];
					      for (col_idx = 11; col_idx < 22; col_idx++)
							  {
						        I2C_SendByte(char_array[ch_idx][col_idx]);
						        while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
                    {
					          }
								    while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
								    {
								    }
					      }
				    } 
		    }						
		}
		I2C_SendSTOP(); //Stop I2C bus
		Delay_ms(50);
}

uint8_t DSPL_Stop(uint8_t Addr)	
{
	   DSPL_FillStr(Addr, clear_string); //Clearing display screen
	   PORT_ResetBits(MDR_PORTC, PORT_Pin_6); //Display power off
     return 0;	
}

void LoadStrArray(uint8_t* Str_Array, uint8_t* Str)
{
	  uint8_t idx;
	
	  for (idx = 0; idx < 11; ++idx) 
	  {   
				Str_Array[idx] = Str[idx];
	  }
}

/* Private functions ------------------------------------- */
void Delay_ms(uint16_t nTime)
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
