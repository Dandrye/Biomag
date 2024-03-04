/*
  ******************************************************************************
  * @file    mmc.c
  * @author  Denis Egorov
  * @version 
  * @date  
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "diskio.h"
#include "MDR32Fx.h"                    // Device header

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */

/* Pin definition */
#define SS   PORT_Pin_0
#define CLK  PORT_Pin_1
#define MISO PORT_Pin_2
#define MOSI PORT_Pin_3

/* Command definition */
#define CMD0	 (0x40+0)  //GO_IDLE_STATE 
#define CMD1	 (0x40+1)	 //SEND_OP_COND (MMC) 
#define	ACMD41 (0xC0+41) //SEND_OP_COND (SDC) 
#define CMD8	 (0x40+8)	 //SEND_IF_COND 
#define CMD16	 (0x40+16) //SET_BLOCKLEN 
#define CMD17	 (0x40+17) //READ_SINGLE_BLOCK 
#define CMD24	 (0x40+24) //WRITE_BLOCK 
#define CMD55	 (0x40+55) //APP_CMD
#define CMD58	 (0x40+58) //READ_OCR 

/* Private macro ----------------------------------------- */
#define SELECT()   PORT_ResetBits(MDR_PORTC, SS)  
#define DESELECT() PORT_SetBits(MDR_PORTC, SS)
#define	SELECTED   (PORT_ReadInputDataBit(MDR_PORTC, SS) == 0)

/* Private variables ------------------------------------- */
PORT_InitTypeDef port;
static BYTE CardType;

/* Private funtion prototypes ---------------------------- */
void SPI_Init(void);
void SPI_SendDummClock(void); //Generate 80 dummy clock pulses
void SPI_SendByte(uint8_t Byte); //Transmitting one byte over SPI
uint8_t SPI_ReceiveByte(void); //Receiving one byte over SPI
uint8_t SPI_SendCommand(uint8_t Cmd, uint32_t Arg); //Sending SD card command

/* Exported functions ------------------------------------ */
DSTATUS disk_initialize(void)
{   
	  uint8_t temp, idx, cmd, sd_type, ocr_array[4];
    uint16_t cnt;	
	
	  SPI_Init();		
		SPI_SendDummClock();   
    SELECT();
    sd_type = 0;
    temp = SPI_SendCommand(CMD0, 0); //Send command CMD0
    if (temp == 0x01) //Idle state
		{
			  temp = SPI_SendCommand(CMD8, 0x01AA); //Send command CMD8
			  if (temp == 0x01) //SD_v2
				{
				    for (idx = 0; idx < 4; idx++) //Getting R7 response
					  {
							  ocr_array[idx] = SPI_ReceiveByte();
						}
            if ((ocr_array[2] == 0x01) && (ocr_array[3] == 0xAA)) //The card can work at Vdd range of 2.7V - 3.6V
						{   
							  cnt = 12000;		
		            do
		            {
			              temp = SPI_SendCommand(CMD55, 0); //Send command CMD55
			 	            if (temp == 0x01)
                    {
                        temp = SPI_SendCommand(ACMD41, 0x40000000); //Send command ACMD41 with HCS bit 				
		                }	
		            }
								while ((temp != 0x00) && (--cnt)); //Wait for leaving idle state
								if (cnt > 0)
								{
									  temp = SPI_SendCommand(CMD58, 0); //Send command CMD58
									  if (temp == 0) 
										{
											  for (idx = 0; idx < 4; idx++)
                        {
													  ocr_array[idx] = SPI_ReceiveByte();
												}
												temp = ocr_array[0];
												if ((temp & 0x40) > 0)
												{																					
					                  sd_type = (CT_SD2 | CT_BLOCK); //HC or SC
												}
												else
												{
													  sd_type = CT_SD2;	//SD_v2 
												}
										}
								}
   					}
            else
            {
                temp = SPI_SendCommand(ACMD41, 0); //Send command ACMD41 without HCS bit (SD_v1 or MMC_v3) 
							  if (temp < 0x01)
								{
								    sd_type = CT_SD1; //SD_v1	 
   									cmd = ACMD41; 
								}
								else
								{
									  sd_type = CT_MMC; //MMC_v3	 
   									cmd = CMD1; 
								}
								cnt = 12000;		
		            do
		            {			              
                    temp = SPI_SendCommand(cmd, 0); //Send command ACMD41 or CMD1		               
		            }
								while ((temp != 0x00) && (--cnt)); //Wait for leaving idle state
								temp = SPI_SendCommand(CMD16, 512); //Send command CMD16
								if (!cnt || temp != 0)
								{
									  sd_type = 0;
								}								
						}							
				}
		}
    CardType = sd_type;
	  DESELECT();
		SPI_ReceiveByte();
		if (sd_type > 0)
		{
			  return  0;
		}
    else 
    {
        return STA_NOINIT;
		}			
}

DRESULT disk_readp(BYTE* Buffer, DWORD SectorNumber, WORD Offset, WORD Count)
{
	  DRESULT result;
	  uint8_t temp;
	  uint16_t cnt;
	
	  if (!(CardType & CT_BLOCK))
		{
		    SectorNumber *= 512; //Convert to byte address if needed	
		}	
		result = RES_ERROR;		
		temp = SPI_SendCommand(CMD17, SectorNumber); //Send command CMD17
    if (temp == 0)
    {
        cnt = 40000;
			  
        /* Wait for data packet	*/		
			  do 
				{
				    temp = SPI_ReceiveByte();   	
				}
				while ((temp == 0xFF) && (--cnt));
				if (temp == 0xFE) //A data packet arrived 
				{
				    cnt = (514 - Offset -	Count);
				}
				
				/* Skip leading bytes */
				if (Offset != 0)
				{
            do
            {
						    SPI_ReceiveByte();
						}	
            while ((--Offset) > 0);						
        }
        
				/* Receive a part of the sector */
				do
				{
				    *Buffer++ = SPI_ReceiveByte();
			  } 
				while ((--Count) > 0);
				
				/* Skip trailing bytes and CRC */
        do 
				{
				    SPI_ReceiveByte();
				}
				while ((--cnt) > 0);
				result = RES_OK;
		}
		DESELECT();
		SPI_ReceiveByte();
	  return result;    
}

#if _USE_WRITE
DRESULT disk_writep(const BYTE* Buffer, DWORD SectorBytes)
{   
    DRESULT res;
	  DWORD   cnt1; //(bc)
	  static  WORD cnt2; //wc
	  uint8_t temp;

	  res = RES_ERROR;

    /* Send data bytes */
	  if (Buffer) 
	  {		
		    cnt1 = SectorBytes;
		    while (cnt1 && cnt2)
		    {		
			      SPI_SendByte(*Buffer++); //Send data bytes to the card
			      cnt1--; cnt2--;
		    }
		    res = RES_OK;
	  } 
	  else 
	  {
		    /* Initiate sector write process */ 
		    if (SectorBytes) 
		    {	
			      if (!(CardType & CT_BLOCK))
            {
 				        SectorBytes *= 512;	//Convert to byte address if needed			
			      }
			
			      /* WRITE_SINGLE_BLOCK */
						temp = SPI_SendCommand(CMD24, SectorBytes);
			      if (temp == 0)
			      {			
				        SPI_SendByte(0xFF); 
				        SPI_SendByte(0xFE); //Data block header
				        cnt2 = 512;				  //Set byte counter
				        res = RES_OK;
			      }
		    }
		    else 
		    {	
			      /* Finalize sector write process */
			      cnt1 = cnt2 + 2;
			      while (cnt1--)
            {		
				        SPI_SendByte(0); //Fill left bytes and CRC with zeros
			      }						
						temp = SPI_ReceiveByte(); //Receive data response and wait for end of write process 
			      if (( temp & 0x1F) == 0x05) 
			      {
				        cnt1 = 5000;
							  temp = 0;
							  while ((temp != 0xFF) && cnt1) //Wait for end of busy state
								{
								    temp = SPI_ReceiveByte();
                    cnt1--;	
                    asm("nop");									
								}
							  if (cnt1)
                {
									  res = RES_OK;
								}
		        }
			      DESELECT();
			      SPI_ReceiveByte();
		    }
    }
	  return res;
}
#endif

/* Private functions ------------------------------------- */
void SPI_Init(void)
{    
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE); //Enables Port C clocking 
	
	  /* Configures Pin2 of Port C DAT0(MISO) in Input Mode */
		port.PORT_Pin       = PORT_Pin_2;
    port.PORT_OE        = PORT_OE_IN; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_ON; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
		port.PORT_PD_SHM    = PORT_PD_SHM_ON;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;		
	  port.PORT_SPEED     = PORT_SPEED_MAXFAST;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTC, &port);

    /* Configures Pin0 CS (SS), Pin1 CLK and Pin3 CMD (MOSI) of Port C in Output Driver Mode */
	  port.PORT_Pin       = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_3; 
	  port.PORT_OE        = PORT_OE_OUT; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_ON; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_SPEED_MAXFAST;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTC, &port);
	
	  PORT_SetBits(MDR_PORTC, MOSI | SS); //MOSI = 1; SS=1    		  
}

void SPI_SendDummClock(void)
{
    uint8_t  idx;
	  
	  for (idx = 0; idx <10; idx++)
	  {
		    SPI_SendByte(0xFF);		
		}    	
}

void SPI_SendByte(uint8_t Byte)
{
	  uint8_t idx;
	
	  for (idx = 0; idx < 8; idx++)
	  {
			  /* Bit formation on the MOSI bus */
			  if ((Byte & 0x80) == 0x00)
				{
					  PORT_ResetBits(MDR_PORTC, MOSI);
				}
				else
				{
					  PORT_SetBits(MDR_PORTC, MOSI);					
				}
				Byte = Byte << 1;
								
				/* Clock generation */
				PORT_SetBits(MDR_PORTC, CLK);    
				asm("nop");
				PORT_ResetBits(MDR_PORTC, CLK);				
		}
}
		
uint8_t SPI_ReceiveByte(void) 
{
	  uint8_t idx, data = 0;
	
	  PORT_SetBits(MDR_PORTC, MOSI);
	  for (idx = 0; idx < 8; idx++)
	  {			  
		    PORT_SetBits(MDR_PORTC, CLK);
			  data = data << 1;			  
			  if (PORT_ReadInputDataBit(MDR_PORTC, MISO) != 0x00)
				{
            data = data | 0x01;
				}
        PORT_ResetBits(MDR_PORTC, CLK);
				asm("nop");
    }
		return data;
}

uint8_t SPI_SendCommand(uint8_t Cmd, uint32_t Arg)
{
	  uint8_t result, n; 

    if ((Cmd & 0x80) != 0) //ACMD<n> is the command sequense of CMD55-CMD<n> 
  	{	
		   Cmd &= 0x7F;
		   result = SPI_SendCommand(CMD55, 0);
		   if (result > 0x01) 
			 { 
				   return result;
			 }
		}  
	 	
	  /* Card selection */
	  DESELECT();
	  SPI_ReceiveByte();
	  SELECT();
	  SPI_ReceiveByte();
	
	  /* Last byte calculation */
	  n = 0x01;
	  if (Cmd == CMD0)   n = 0x95;
    if (Cmd == CMD8)   n = 0x87;
	  if (Cmd == ACMD41)
    {			
			  if (Arg == 0) n = 0xE5;
			  else n = 0x77;
		}
	  if (Cmd == CMD55)  n = 0x65;
	
	  /* Sending a command packet */
	  SPI_SendByte(Cmd);
	  SPI_SendByte((uint8_t)(Arg >> 24));
	  SPI_SendByte((uint8_t)(Arg >> 16)); 
	  SPI_SendByte((uint8_t)(Arg >> 8));
	  SPI_SendByte((uint8_t)Arg);
	  SPI_SendByte(n);
	 
	  /* Wating for response */
		n = 10;
		do
		{
				result = SPI_ReceiveByte();		   
	  } 
		while (((result & 0x80) != 0x00) && (--n));
	  return result;		
}	






