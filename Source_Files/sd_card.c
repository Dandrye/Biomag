/*
  ******************************************************************************
  * @file    sd_card.c
  * @author  Denis Egorov
  * @version V1.5.1
  * @date    01/29/2021
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "sd_card.h"
#include "pff.h"

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
WORD byte_cnt;

/* Private funtion prototypes ---------------------------- */
RW_RESULT ReadSD(const char* FileName, char* Buff, uint16_t ByteToRead); //Reading data from the file to the buffer array
RW_RESULT WriteSD(const char* FileName, char* Buff, uint16_t ByteToWrite, uint32_t Offset); //Writing data from the buffer array to the file
void DateTimeToStr(struct BMData* DataTime, char* Array, uint8_t* Offset); //Converting date and time to the char array
void DataToStr(struct BMData* Data, char* Array, uint8_t* Offset); //Converting data to the char array
void ID_ToStr(struct BMData* Data, char* Array, uint8_t* Offset); //Converting product ID to the char array
	
/* Exported functions ------------------------------------ */
void ReadCfgFile(const char* FileName, struct BCData* Cfg)
{
    uint8_t idx = 0;
    uint16_t	temp;
	  uint32_t result;
	  RW_RESULT res;
	  char read_buff[32] = {0};
	
	  do
    {			 
		    res = ReadSD(FileName, read_buff, 32); 
		   idx++;
    }
		while ((res != OK) && (idx < 2));
    if (res == OK)
    {   
			  /* Read and write ID */
			  temp = (read_buff[0] - 0x30) * 10;
		    temp += read_buff[1] - 0x30;
		    Cfg->ID = temp;			
			
		    /* Read day */
        temp = (read_buff[12] - 0x30) * 10;
		    temp += read_buff[13] - 0x30;
		    result = temp;
        result = result << 8;			 
				 			        		
        /* Read month */
        temp = (read_buff[8] - 0x30) * 10;
        temp += read_buff[9] - 0x30;					 
				result |= temp;
        result = result << 8;	
				 
			  /* Read year and write date */
			  temp = (read_buff[4] - 0x30) * 10;
        temp += read_buff[5] - 0x30;							   		   				   
				result |= temp;
				Cfg->Date = result;				 
								 
				/* Read hour */
				temp = (read_buff[16] - 0x30) * 10;
			  temp += read_buff[17] - 0x30;
			  result = temp;
        result = result << 8;	
				 
				/* Read minute and write time */
				temp = (read_buff[20] - 0x30) * 10;
        temp += read_buff[21] - 0x30;							   		   				   
				result |= temp;
				Cfg->Time = result;		
								 
				/* Read and write temperature polling time */
				temp = (read_buff[24] - 0x30) * 100;
			  temp += (read_buff[25] - 0x30) * 10;
				temp += read_buff[26] - 0x30; 
			  Cfg->PolTime_T = temp;
				 
				/* Read and write magnetic strenge polling time */
				temp = (read_buff[29] - 0x30) * 100;
			  temp += (read_buff[30] - 0x30) * 10;
				temp += read_buff[31] - 0x30; 
			  Cfg->PolTime_M = temp;
 		} 
    else
    {
        Cfg->ID = 0;
			  Cfg->Date = 0x00010116;
			  Cfg->Time = 0;
			  Cfg->PolTime_T = 0x168; 
        Cfg->PolTime_M = 0xB4;			 
		}		 		
}

void WriteDataFile(const char* FileName, struct BMData* Data, uint32_t* FPtr)
{
    uint8_t idx = 0, offset = 2;
	
	  RW_RESULT res = ERR;
	  char write_buff[74] = {0};
		write_buff[0] = 0x0D; //Return carret 
		write_buff[1] = 0x0A; //New line
		ID_ToStr(Data, write_buff, &offset);
    DateTimeToStr(Data, write_buff, &offset);
		DataToStr(Data, write_buff, &offset);
		do
    {			 
		    res = WriteSD(FileName, write_buff, offset, *FPtr); 
			  idx++;
    }
		while ((res != OK) && (idx < 2));
    if (res == OK)
		{
			 *FPtr += 512;
       if (*FPtr >= DATA_FILE_SIZE)
       {
           *FPtr = 0;
			 }				 
		}
}

/* Private functions ------------------------------------- */
RW_RESULT ReadSD(const char* FileName, char* Buff, uint16_t ByteToRead)
{   
    FATFS fs;	
    FRESULT res, res1;

		res =  pf_mount(&fs);
	  if (res == FR_OK)
		{
		    res = pf_open(FileName);
			  if (res == FR_OK)
				{
				    res = pf_lseek(0);
					  if (res == FR_OK)
						{
           	    res = pf_read(Buff, ByteToRead, &byte_cnt);							       
						}
				}
				res1 = pf_mount(0x00);	
		}
    if ((res | res1) != 0) return ERR;
    else return OK;  			
}

RW_RESULT WriteSD(const char* FileName, char* Buff, uint16_t ByteToWrite, uint32_t Offset)
{
    FATFS fs;	
    FRESULT res, res1;
	
	  res =  pf_mount(&fs);
	  if (res == FR_OK)
		{
		    res = pf_open(FileName);
		    if (res == FR_OK)
				{
			      res = pf_lseek(0 + Offset);
				    if (res == FR_OK)
			      {
                res = pf_write(Buff, ByteToWrite, &byte_cnt);	
					      res = pf_write(0, 0, &byte_cnt);
					  }
				}
				res1 = pf_mount(0x00);
		}
		if ((res | res1) != 0) return ERR;
    else return OK;  					
}	

void DataToStr(struct BMData* Data, char* Array, uint8_t* Offset)
{
    uint8_t idx, mode;
	  int16_t temp;
	
    idx = *Offset;
	  for (mode = 0; mode < 7; mode++)
    { 	
	      switch (mode)
		    {
			      case 0:
            {
                temp = Data->Hx;
					      break;					
			      }
            case 1:
            {
                temp = Data->Hy;
					      break;					
			      } 
			      case 2:
            {
                temp = Data->Hz;
					      break;					
			      }
            case 3:
            {
                temp = Data->Hx_var;
					      break;					
			      }
            case 4:
            {
                temp = Data->Hy_var;
					      break;					
			      } 
			      case 5:
            {
                temp = Data->Hz_var;
					      break;					
			      }	
            case 6:
            {
					      temp = Data->Tmpr;
					      break;					
			      }				
		    }
        if ((mode != 3) || (mode	!= 4) || (mode	!= 5))	
		    {
		        if ((temp & SIGN_MSK) != 0)
		        {
		            Array[idx] = 0x2D; //"-" 
                temp = ~temp;
			          temp += 1;
                idx++;					
		        }		    
		    }
		    Array[idx] = (uint8_t)(temp/1000) + 0x30;
		    temp = temp%1000;
		    idx++;
		    Array[idx] = (uint8_t)(temp/100) + 0x30;
		    temp =  temp%100;
		    idx++;
		    Array[idx] = 0x2E; //"."
		    idx++;
        Array[idx] = (uint8_t)(temp/10) + 0x30;
				idx++;
				Array[idx] = 0x0D; //Return carret
				idx++;
				Array[idx] = 0x0A; //New Line
				idx++;
	  } 
    *Offset = idx;		
}

void DateTimeToStr(struct BMData* DataTime, char* Array, uint8_t* Offset)
{
	  int8_t idx1;
	  uint8_t  idx, mode, temp, *pntr1;
	  uint32_t temp2, *pntr2;
	 	
    pntr2 = &temp2;
	  pntr1 = (uint8_t*)pntr2;
	  idx = *Offset;
	  for (mode = 0; mode < 2; mode++)
    { 	
	      switch (mode)
		    {
			      case 0:
            {
                temp2 = DataTime->Date;
							  idx1 = 2;
					      break;					
			      }
            case 1:
            {
                temp2 = DataTime->Time;
							  idx1 = 1; 
					      break;					
			      }			      
		    }
				do
				{
				   temp = *(pntr1 + idx1);
				   Array[idx] = (uint8_t)(temp/10) + 0x30;
				   idx++;
				   Array[idx] = (uint8_t)(temp%10) + 0x30;
					 idx1--;
					 idx++;
					 Array[idx] = 0x0D; //Return carret 
		       idx++;
		       Array[idx] = 0x0A; //New line
		       idx++;
				}
				while ((idx1 + 1) != 0);						
	  }
    *Offset = idx;		    
}

void ID_ToStr(struct BMData* Data, char* Array, uint8_t* Offset)
{
    uint8_t idx, temp;
	  	
	  idx = *Offset;
	  temp = Data->ID;
	  Array[idx] = (temp/10) + 0x30;
		temp = (temp%10) + 0x30;
		idx++;
	  Array[idx] = temp;
	  idx++;
		Array[idx] = 0x0D; //Return carret 
		idx++;
	  Array[idx] = 0x0A; //New line
	  idx++;	
	  *Offset = idx;	
}


