/*
  ******************************************************************************
  * @file    meas.c
  * @author  Denis Egorov
  * @version V1.20
  * @date    02/15/2022
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "meas.h"

/* Private typedif --------------------------------------- */ 

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
struct BMData basic_data = {0};
struct CalibData calib_factor = {0};
uint16_t meas_data[6] = {0}; 
uint16_t temp_data_x[100] = {0};
uint16_t temp_data_y[100] = {0};
uint16_t temp_data_z[100] = {0};

/* Private funtion prototypes ---------------------------- */
uint8_t GetRegVal(uint8_t IC_Addr, uint8_t Reg_Addr); //Getting ID of sensor's IC
bool CheckIC_ID(uint8_t IC_Addr, uint8_t Reg_Addr, uint8_t ID1, uint8_t ID2); //Checking if IC ID is correct
void WriteReg(uint8_t IC_Addr, uint8_t Reg_Addr, uint8_t Value); //Writing to the register
int32_t GetActualInd(uint16_t Data, struct CalibData Factor, uint8_t Mode); //Calculating the actual magnetic induction
void Delay_MS(uint16_t nCount);//Adjustable delay time function

/* Exported functions ------------------------------------ */
bool CheckSensorsInterface(uint8_t* Str_Array)
{
	  uint8_t err_cnt, Idx;
	
	  err_cnt = 0;
	  if (CheckIC_ID(BMP_ADDR, BMP_ID_ADDR, BMP180_ID, BMP280_ID) == FALSE) err_cnt++;
		if (CheckIC_ID(MPU_ADDR, MPU_ID_ADDR, MPU9250_ID, MPU9255_ID) == FALSE) err_cnt += 2;
		switch (err_cnt)
		{   
			  case 0: for (Idx = 0; Idx < 11; ++Idx) 
			          {   
								    Str_Array[Idx] = start_string[Idx];
								}	 
			          break; 
        case 1: for (Idx = 0; Idx < 11; ++Idx) 
			          {   
								    Str_Array[Idx] = t_ic_err_string[Idx];
								}	 
			          break;
        case 2: for (Idx = 0; Idx < 11; ++Idx) 
			          {   
								    Str_Array[Idx] = m_ic_err_string[Idx];
								}	 
				        break;
        case 3: for (Idx = 0; Idx < 11; ++Idx) 
			          {   
								    Str_Array[Idx] = ic_err_string[Idx];
								}	 
			          break; 
    }			
		if (err_cnt == 0)
    {
 		    return TRUE;
		}
    else
    {
			  return FALSE;	
		}			
}

void ConfigTmprSensor(void)
{
    WriteReg(BMP_ADDR, BMP_CONFIG_ADDR, INIT_VALUE_TMPR);
	  WriteReg(BMP_ADDR, BMP_CONTROL_ADDR, SLEEP_MODE_TMPR);
}

void ReadTmprCompData(struct CalibData* Factor)
{
	  uint8_t temp_lsb, temp_msb;
	  
	  /* Getting compensation coefficient T1 */
	  temp_lsb = GetRegVal(BMP_ADDR, BMP_T1_LSB_ADDR);
	  temp_msb = GetRegVal(BMP_ADDR, BMP_T1_MSB_ADDR);
	  Factor->T1 = temp_msb << 8 | temp_lsb;
	
	  /* Getting compensation coefficient T2 */	
	  temp_lsb = GetRegVal(BMP_ADDR, BMP_T2_LSB_ADDR);
	  temp_msb = GetRegVal(BMP_ADDR, BMP_T2_MSB_ADDR);
	  Factor->T2 = temp_msb << 8 | temp_lsb;
	
	  /* Getting compensation coefficient T3 */	
	  temp_lsb = GetRegVal(BMP_ADDR, BMP_T3_LSB_ADDR);
	  temp_msb = GetRegVal(BMP_ADDR, BMP_T3_MSB_ADDR);
	  Factor->T3 = temp_msb << 8 | temp_lsb;         
}

void ReadTmprData(uint16_t* MeasValue)
{
    uint8_t temp_lsb, temp_msb;
	
	  /* Start the measurement */
	  WriteReg(BMP_ADDR, BMP_CONTROL_ADDR, FORSED_MODE_TMPR);
    Delay_MS(10);
	
	  /* Getting measured temperature */
	  temp_lsb = GetRegVal(BMP_ADDR, BMP_T_LSB_ADDR);
	  temp_msb = GetRegVal(BMP_ADDR, BMP_T_MSB_ADDR);
	  MeasValue[0] = (temp_msb << 8) | temp_lsb;
	
}	

int16_t GetActualTmpr(uint16_t* MeasValue, struct CalibData Factor)
{
    int32_t temp1, temp2, temp3;
    
    /*Calculating the actual temperature value*/	
	  temp3 = MeasValue[0] - Factor.T1;
    temp1 = temp3*Factor.T2;
	  temp1 = temp1 >> 10;
    temp2 = ((temp3*temp3) >> 12)*Factor.T3;
    temp2 = temp2 >> 14; 	
	  temp3 = temp1+temp2;
	  temp1 = ((5*temp3) + 128) >> 8;
    return  (int16_t)((((temp1 + TMPR_OFF) * TMPR_MUL)/TMPR_DIV) + TMPR_ADD);	
}	
void ConfigMagSensor(void)
{
    WriteReg(MPU_ADDR, PWR_MGMT_1_ADDR, 0x80);
	  Delay_MS(1);
  	WriteReg(MPU_ADDR, PWR_MGMT_1_ADDR, 0x18);
	  //WriteReg(MPU_ADDR, SMPLRT_DIV_ADDR, 0x07);
	  //WriteReg(MPU_ADDR, CONFIG_ADDR, 0x06);
    //WriteReg(MAG_ADDR, CNTL_2_ADDR, 0x01);
	  Delay_MS(1);
}

void ReadMagAdjData(struct CalibData* Factor)
{
    WriteReg(MPU_ADDR, INT_BYPASS_ADDR, 0x02); //Switching the bypass on
    Delay_MS(1);
	  WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x0F); //Setting Fuse ROM access mode
	  Delay_MS(1);	  
    Factor->SAX = GetRegVal(MAG_ADDR, ASA_X_ADDR); //Getting magnetic sensor X-axis sensitivity adjustment value
	  Factor->SAY = GetRegVal(MAG_ADDR, ASA_Y_ADDR); //Getting magnetic sensor Y-axis sensitivity adjustment value 
	  Factor->SAZ = GetRegVal(MAG_ADDR, ASA_Z_ADDR); //Getting magnetic sensor Z-axis sensitivity adjustment value 
	  Delay_MS(1);
}	

void ReadMagData(uint16_t* MeasValue, uint8_t AvrNum)
{
    uint8_t Idx, temp_lsb, temp_msb;
	  int16_t temp; 
	  int32_t temp_buff[6] = {0};
	  	
	  /* Start the measurement */		
	  //temp = 0; 
   	WriteReg(MPU_ADDR, INT_BYPASS_ADDR, 0x02); //Switching the bypass on
    Delay_MS(1);
	  WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x00); //Setting Power-Down mode
	
	  for (Idx =0; Idx < AvrNum; Idx++)
    { 	
			  temp = 0; 
	      Delay_MS(1);
	      WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x01); //Setting Single-Measurement mode	
	      while ((temp & 0x01) != 0x01) //Waiting for data ready
		    {
			      temp = GetRegVal(MAG_ADDR, ST_1_ADDR);
		    }    
	
        /* Getting magnetic field data for axis X */	
	      temp_lsb = GetRegVal(MAG_ADDR, MPU_X_LSB_ADDR);
	      temp_msb = GetRegVal(MAG_ADDR, MPU_X_MSB_ADDR);
				temp = (temp_msb << 8) | temp_lsb;
				   temp_data_x[Idx] = temp;
	      temp_buff[0] = temp_buff[0] + temp;
				if ((temp & 0x8000) > 0)
				{
					  temp = ~temp;
					  temp += 1;					
				}
			  if (temp > (int16_t)temp_buff[3])
        {
            temp_buff[3] = temp; 
				}					
					
	      /* Getting magnetic field data for axis Y */
	      temp_lsb = GetRegVal(MAG_ADDR, MPU_Y_LSB_ADDR);
	      temp_msb = GetRegVal(MAG_ADDR, MPU_Y_MSB_ADDR);
				temp = (temp_msb << 8) | temp_lsb;
				   temp_data_y[Idx] = temp;
	      temp_buff[1] = temp_buff[1] + temp;
				if ((temp & 0x8000) > 0)
				{
					  temp = ~temp;
					  temp += 1;					
				}
			  if (temp > (int16_t)temp_buff[4])
        {
	          temp_buff[4] = temp; 
				}					
								
	      /* Getting magnetic field data for axis Z */
	      temp_lsb = GetRegVal(MAG_ADDR, MPU_Z_LSB_ADDR);
	      temp_msb = GetRegVal(MAG_ADDR, MPU_Z_MSB_ADDR);
				temp = (temp_msb << 8) | temp_lsb;
				   temp_data_z[Idx] = temp;
	      temp_buff[2] = temp_buff[2] + temp;
				if ((temp & 0x8000) > 0)
				{
					  temp = ~temp;
					  temp += 1;					
				}
			  if (temp > (int16_t)temp_buff[5])
        {
	          temp_buff[5] = temp; 
				}					
		
	      /* Stop the measurement */
	      GetRegVal(MAG_ADDR, ST_2_ADDR); 
        WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x00);//Setting Power-Down mode	
				//Delay_MS(8);
	  }
		MeasValue[0] = temp_buff[0]/AvrNum;;
		MeasValue[1] = temp_buff[1]/AvrNum;;
		MeasValue[2] = temp_buff[2]/AvrNum;;
		
		temp = MeasValue[0];
		if ((temp & 0x8000) > 0)
		{
				temp = ~temp;
			  temp += 1;					
		}
		MeasValue[3] = (int16_t)temp_buff[3] - temp;
		
		temp = MeasValue[1];
		if ((temp & 0x8000) > 0)
		{
				temp = ~temp;
			  temp += 1;					
		}
		MeasValue[4] = (int16_t)temp_buff[4] - temp;
		
		temp = MeasValue[2];
		if ((temp & 0x8000) > 0)
		{
				temp = ~temp;
			  temp += 1;					
		}
		MeasValue[5] = (int16_t)temp_buff[5] - temp;
}

void GetMagStrength(uint16_t* MeasValue, struct BMData* Data)
{
    uint8_t Idx;
	  int32_t temp_array[6] = {0};
	  int32_t temp;

    temp_array[0] = GetActualInd(MeasValue[0], calib_factor, MAG_X_MODE);
	  temp_array[1] = GetActualInd(MeasValue[1], calib_factor, MAG_Y_MODE);
		temp_array[2] = GetActualInd(MeasValue[2], calib_factor, MAG_Z_MODE);
		temp_array[3] = GetActualInd(MeasValue[3], calib_factor, MAG_X_VAR_MODE);
	  temp_array[4] = GetActualInd(MeasValue[4], calib_factor, MAG_Y_VAR_MODE);
		temp_array[5] = GetActualInd(MeasValue[5], calib_factor, MAG_Z_VAR_MODE);	
		
	  /* Calculating the magnetic feild strength */
    for (Idx = 0; Idx < 6; ++Idx)
    {
		    temp = temp_array[Idx] * 4776;
        temp = temp/1000;
        switch (Idx)
        {
            case 0: Data->Hx = (int16_t)(((temp * MAG_X_MUL)/MAG_X_DIV) + MAG_X_ADD);
					          break;
            case 1: Data->Hy = (int16_t)(((temp * MAG_Y_MUL)/MAG_Y_DIV) + MAG_Y_ADD);   			      
			              break; 
            case 2: Data->Hz = (int16_t)(((temp * MAG_Z_MUL)/MAG_Z_DIV) + MAG_Z_ADD);			      
			              break;
					  case 3: Data->Hx_var = (int16_t)(((temp * MAGV_X_MUL)/MAGV_X_DIV) + MAGV_X_ADD); 			      
			              break; 
            case 4: Data->Hy_var = (int16_t)(((temp * MAGV_Y_MUL)/MAGV_Y_DIV) + MAGV_Y_ADD); 	  			      
			              break; 
            case 5: Data->Hz_var = (int16_t)(((temp * MAGV_Z_MUL)/MAGV_Z_DIV) + MAGV_Z_ADD); 	  			      
			              break;
        }					
		}			
}

void MagSelfTest(uint16_t* MeasValue)
{
    uint8_t temp, temp_lsb, temp_msb; 
	  int16_t temp1;
	
		/* Start the self test */  	
	  temp = 0; 
	  WriteReg(MPU_ADDR, INT_BYPASS_ADDR, 0x02); //Switching the bypass on
    Delay_MS(10);
    WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x00); //Setting Power-Down mode
    WriteReg(MAG_ADDR, ASTC_ADDR, 0x40); //Generating magnetic field for self-test
	  WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x08); //Setting Self-Test mode
	  while ((temp & 0x01) != 0x01) //Waiting for data ready
		{
			  temp = GetRegVal(MAG_ADDR, ST_1_ADDR);
		}		
	 		
	  /* Getting magnetic field data for axis X */	
	  temp_lsb = GetRegVal(MAG_ADDR, MPU_X_LSB_ADDR);
	  temp_msb = GetRegVal(MAG_ADDR, MPU_X_MSB_ADDR);
	  MeasValue[0] = (temp_msb << 8) | temp_lsb;
	
	  /* Getting magnetic field data for axis Y */
	  temp_lsb = GetRegVal(MAG_ADDR, MPU_Y_LSB_ADDR);
	  temp_msb = GetRegVal(MAG_ADDR, MPU_Y_MSB_ADDR);
	  MeasValue[1] = (temp_msb << 8) | temp_lsb;
	
	  /* Getting magnetic field data for axis Z */
	  temp_lsb = GetRegVal(MAG_ADDR, MPU_Z_LSB_ADDR);
	  temp_msb = GetRegVal(MAG_ADDR, MPU_Z_MSB_ADDR);
	  MeasValue[2] = (temp_msb << 8) | temp_lsb;
		
	  temp1 = MeasValue[2]/5;
	  MeasValue[2] = temp1;
		 
		/* Stop the self test */
	  WriteReg(MAG_ADDR, ASTC_ADDR, 0x00); //Stop generating magnetic field for self-test
	  WriteReg(MAG_ADDR, CNTL_1_ADDR, 0x00); //Setting Power-Down mode
}	

/* Private functions ------------------------------------- */
uint8_t GetRegVal(uint8_t IC_Addr, uint8_t Reg_Addr)
{  
    uint8_t IC_ID;
	
	  while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) //Checking if I2C bus is free
    {
    }
	  I2C_Send7bitAddress(IC_Addr, I2C_Direction_Transmitter); //Sending IC address
	  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) //Waiting end of transfer
    {
    }
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
    { 
			  I2C_SendByte(Reg_Addr); //Sending ID's register address
				while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) //Waiting end of transfer
        {
        }
		    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
        {
						I2C_Send7bitAddress(IC_Addr, I2C_Direction_Receiver); //Sending IC address
	          while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) //Waiting end of transfer
            {
            }
		       	if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
            {
						    I2C_StartReceiveData(I2C_Send_to_Slave_NACK); //Receiving byte and sending non confirmation signal
                while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) //Waiting end of transfer
                {
                }
                IC_ID = I2C_GetReceivedData(); //Getting data from I2C RXD register 					
						}					
				}	    	
		}
		I2C_SendSTOP(); //Stop I2C bus 
    return IC_ID;   	
}

void WriteReg(uint8_t IC_Addr, uint8_t Reg_Addr, uint8_t Value)
{
     while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) //Checking if I2C bus is free
    {
    }
	  I2C_Send7bitAddress(IC_Addr, I2C_Direction_Transmitter); //Sending IC address
	  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
    {
    }
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
    { 
			  I2C_SendByte(Reg_Addr); //Sending the register address
			  while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
        {
        }
		    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) //Checking the confirmation signal
        {
				    I2C_SendByte(Value);//Sending data to the register
		       	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
            {
            }		
			  }	
		}
		I2C_SendSTOP(); //Stop I2C bus  
}	

bool CheckIC_ID(uint8_t IC_Addr, uint8_t Reg_Addr, uint8_t ID1, uint8_t ID2)
{
    uint8_t IC_ID;
	  
	  IC_ID = GetRegVal(IC_Addr, Reg_Addr); //Getting value of IC ID
	
	  /* Checking if IC ID is correct or not */
	  if ((IC_ID == ID1) || (IC_ID == ID2)) return TRUE;
	  else return FALSE; 
}

int32_t GetActualInd(uint16_t Data, struct CalibData Factor, uint8_t Mode)
{
	  uint8_t temp1;
	  int32_t temp2;
	
	  switch (Mode)
		{
			  case MAG_X_MODE:     temp1 = Factor.SAX;   			      
			                       break;
			  case MAG_X_VAR_MODE: temp1 = Factor.SAX;   			      
			                       break;
			  case MAG_Y_MODE:     temp1 = Factor.SAY;   			      
			                       break; 
			  case MAG_Y_VAR_MODE: temp1 = Factor.SAY;   			      
			                       break;  
			  case MAG_Z_MODE:     temp1 = Factor.SAZ;   			      
		                         break;      
			  case MAG_Z_VAR_MODE: temp1 = Factor.SAZ;   			      
			                       break;   			
		}			
	  temp2 = (50*temp1) - 6400;
		temp2 = (temp2/128) + 100;
		temp2 =  temp2 * (int16_t)(Data);
	  return temp2/10;	
}	

void Delay_MS(uint16_t nCount)
{
   uint32_t temp;
  
   temp = 80000*nCount;	
	 for (; temp != 0; temp--);
}



