/*
  ******************************************************************************
  * @file    usb.c
  * @author  Denis Egorov
  * @version V1.0
  * @date    05/26/2022
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "usb.h"

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */
#define BUFFER_LENGTH 10

/* Private macro ----------------------------------------- */
USB_Clock_TypeDef usb_clock;
USB_DeviceBUSParam_TypeDef usb_bus;
USB_CDC_LineCoding_TypeDef vcom_param;
uint8_t buffer[BUFFER_LENGTH];
bool usb_req;
uint32_t buf_length;

/* Private funtion prototypes ---------------------------- */
void VCom_Configuration(void); //Virtual COM port configuration
bool CheckCRC(uint8_t Buff[]); //CRC calculation and check

/* Interrupts handling ----------------------------------- */

/* Exported functions ------------------------------------ */
void USB_Configuration(void)
{
    VCom_Configuration(); //Set the communication parameters with a virtual COM port
	
    USB_CDC_Init(buffer, 1, SET); //CDC layer initialization 
	
    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE); //Enables the CPU_CLK clock on USB

    /* Device layer initialization */
    usb_clock.USB_USBC1_Source = USB_C1HSEdiv2;
    usb_clock.USB_PLLUSBMUL    = USB_PLLUSBMUL12;

    usb_bus.MODE  = USB_SC_SCFSP_Full;
    usb_bus.SPEED = USB_SC_SCFSR_12Mb;
    usb_bus.PULL  = USB_HSCR_DP_PULLUP_Set;

    USB_DeviceInit(&usb_clock, &usb_bus);
    
    USB_SetSIM(USB_SIS_Msk); //Enable all USB interrupts
    USB_DevicePowerOn();    
    NVIC_EnableIRQ(USB_IRQn); //Enable interrupt on USB

    USB_DEVICE_HANDLE_RESET;	
}

void USB_Deinit(void)
{
	  NVIC_DisableIRQ(USB_IRQn); //Disableable interrupt on USB
	  USB_DevicePowerOff();
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, DISABLE); //Disables the CPU_CLK clock on USB
}

bool USB_Connect(void)
{   
	  if ((buffer[0] == 0x9) && (buffer[1] == 0xA))
		{
				buffer[0] = 0xA;
				buffer[1] = 0x9;
		    USB_CDC_SendData(buffer, buf_length);
        return TRUE;			    
		}		
    return FALSE;		
}	

bool USB_WriteCfg(struct BCData* Cfg)
{
	  uint32_t temp;
	
	  if (CheckCRC(buffer) == TRUE)
	  {
        /* Write ID */
        Cfg->ID = buffer[0]; 
	
		    /* Read day */
        temp = buffer[3];
		    temp = temp << 8;	
					 			        		
        /* Read month */
        temp |= buffer[2];    
		    temp = temp << 8;	
				 
		    /* Read year and write date */
		    temp |= buffer[1];    			   		   				   
	      Cfg->Date = temp; 
	
	      /* Read hour */
	      temp = buffer[4];
		    temp = temp << 8;	
				 
		    /* Read minute and write time */
		    temp |=  buffer[5];   			   		   				   
		    Cfg->Time = temp;		
	
	      /* Read and write temperature polling time */
		    temp = buffer[6];
		    temp = temp << 8;
		    temp |= buffer[7];
		    Cfg->PolTime_T = temp;
				 
		    /* Read and write magnetic strenge polling time */
		    Cfg->PolTime_M = buffer[8]; 
        return TRUE;
		}else return FALSE;			
}

/* Private functions --------------------------------------*/
void VCom_Configuration(void)
{
	 vcom_param.dwDTERate = 9599;
   vcom_param.bCharFormat = 0;
   vcom_param.bParityType = 0;
   vcom_param.bDataBits = 8;
}

USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length)
{
    buf_length = Length;
	  usb_req = TRUE;
		return USB_SUCCESS;
}

USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA)
{
    assert_param(DATA);
    
    if (wINDEX != 0)
    {
        return USB_ERR_INV_REQ; //Invalid interface
    }

    *DATA = vcom_param; //Store received settings
    
    return USB_SUCCESS;
}

USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA)
{
    assert_param(DATA);
    
    if (wINDEX != 0)
    {       
       return USB_ERR_INV_REQ;  //Invalid interface
    }

    vcom_param = *DATA; //Send back settings stored earlier
    
    return USB_SUCCESS;
}

bool CheckCRC(uint8_t Buff[])
{
	uint8_t i, j, temp = 0;
	
	for (i = 0; i < (BUFFER_LENGTH - 1); i++) 
	{
		  for (j = 7; j > 0; j--)
     	{
				  temp = ((temp << 1) | ((Buff[i] >> j) & 0x01)); 
		      if ((temp & 0x80) > 0)
					{
						  temp = ((temp & 0x7F) ^ 0x09);
					}			
	    }		
  }
	if (temp == Buff[BUFFER_LENGTH - 1]) return TRUE;
	else return FALSE;	
}	
