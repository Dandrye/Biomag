/*
  ******************************************************************************
  * @file    adc.c
  * @author  Denis Egorov
  * @version V1.3
  * @date    02/21/2022
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "adc.h"

/* Private typedif --------------------------------------- */

/* Private define ---------------------------------------- */

/* Private macro ----------------------------------------- */

/* Private variables ------------------------------------- */
ADC_InitTypeDef  adc;
ADCx_InitTypeDef adcx;

/* Private funtion prototypes ---------------------------- */
uint16_t GetADC_Data(void); //Battery level measurement

/* Interrupts handling ----------------------------------- */

/* Exported functions ------------------------------------ */
void ADC_Configuration(void)
{
     RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE); //Enable ADC clocking	
	   ADC_DeInit(); //Reset all ADC settings
	   ADC_StructInit(&adc);
	
     adc.ADC_SynchronousMode   = ADC_SyncMode_Independent;
     adc.ADC_TempSensor        = ADC_TEMP_SENSOR_Disable;
     ADC_Init(&adc);

     ADCx_StructInit(&adcx);
	   adcx.ADC_ClockSource   = ADC_CLOCK_SOURCE_CPU; 
     adcx.ADC_SamplingMode  = ADC_SAMPLING_MODE_SINGLE_CONV; 
     adcx.ADC_ChannelNumber = ADC_CH_ADC2;
	   adcx.ADC_VRefSource    = ADC_VREF_SOURCE_EXTERNAL;
	   ADC1_Init(&adcx);	
}

void GetChargeLevel(uint8_t* Str_Array)
{
	  int8_t  idx, idx1 = 0;
	  int16_t temp;
	
	  for (idx = 0; idx < 11; ++idx)
	  {
        Str_Array[idx] = 0x1C;			 
		}		
	
	  temp = GetADC_Data(); //Getting ADC measurement data	 
	  
	  /* Calculating battery charge level */
	  temp -= LOW_LEVEL;
		if (temp <= 0)
		{
			 temp = LOW_LEVEL;
		}
	  temp = (10*temp)/(HIGH_LEVEL - LOW_LEVEL);
	
	  /* Filling the string */
	  for (idx = 10; (temp - idx1) > 0; idx--)
	  {
        Str_Array[idx] = 0x18;
			  idx1++;
		}	
}

/* Private functions ------------------------------------- */
uint16_t GetADC_Data(void)
{
	  uint8_t idx;
	  uint32_t temp = 0, temp1;
	
	  ADC1_Cmd(ENABLE);
	  for (idx = 0; idx < 10; idx++)
	  {
			  ADC1_Start();	
			  while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) != SET)
        {					
				}
        temp1 = ADC1_GetResult();
				temp1 = (temp1 & ADC_RESULT_MSK);	
        temp +=	temp1;			
		}
		ADC1_Cmd(DISABLE);
    temp = (uint16_t)(temp/10); 		
    return temp;	
}

