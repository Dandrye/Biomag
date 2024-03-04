/*
  ******************************************************************************
  * @file    init.c
  * @author  Denis Egorov
  * @version V3.00 
  * @date    05/26/2022
  * @brief   
  ******************************************************************************
*/

/* Includes ---------------------------------------------- */
#include "init.h"
#include "adc.h"
#include "usb.h"

/* Private typedif --------------------------------------- */


/* Private define ---------------------------------------- */


/* Private macro ----------------------------------------- */


/* Private variables ------------------------------------- */
extern PORT_InitTypeDef  port;
I2C_InitTypeDef   i2c;
TIMER_CntInitTypeDef timer;

/* Private funtion prototypes ---------------------------- */
void RST_Configuration(void); //RST configuration function
void I2C_Configuration(void); //I2C configuration function
void RTC_Cofiguration(void);  //RTC configuration function

/* Exported functions ------------------------------------ */

void Sys_Init(void)
{
	  RST_Configuration();
	  Pin_Configuration();
	  I2C_Configuration();
	  ADC_Configuration();
	  TIM_Configuration();
	  USB_Configuration();
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP,ENABLE); //Enables the HSE clock for BKP control
}

void BKP_Init(struct BCData* CntVal)
{
	  uint16_t temp; 
	  uint32_t count_value;
	
	  /* Calculating the initial value of the RTC */
	  temp = CntVal->Time;
	  count_value = 60*(temp & 0x00FF);
	  count_value += 3600*(temp >> 8);
	 	
    //RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP,ENABLE); //Enables the HSE clock for BKP control 
	
	  /* RTC reset */
    BKP_RTC_Reset(ENABLE);
    BKP_RTC_Reset(DISABLE);	
              
		RST_CLK_LSEconfig(RST_CLK_LSE_ON); //Enable LSE
	  while (RST_CLK_LSEstatus() != SUCCESS) //Good LSE clock
		{
		}
		BKP_RTCclkSource(BKP_RTC_LSEclk); //Select LSE clock as BKP clock source 
	 
    /* Set the RTC prescaler value */
		BKP_RTC_WaitForUpdate(); 
    BKP_RTC_SetPrescaler(RTC_PRESCALER_VALUE);
									
		/* Set the RTC alarm value */
    BKP_RTC_WaitForUpdate();
    BKP_RTC_SetAlarm(count_value + ALARM_VALUE);
		
		/* Set the RTC counter value */
    BKP_RTC_WaitForUpdate();
    BKP_RTC_SetCounter(count_value);
		
		/* RTC enable */
		BKP_RTC_WaitForUpdate(); 
		BKP_RTC_Enable(ENABLE); 
		
		/* Set sleep Mode */
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; //Setting of "Deep Sleep" mode	bit 
		
    /* Enable the BKP interrupts */	  
		BKP_RTC_ITConfig(BKP_RTC_IT_ALRF | BKP_RTC_IT_OWF, ENABLE);
		//BKP_RTC_ITConfig(BKP_RTC_IT_SECF | BKP_RTC_IT_ALRF | BKP_RTC_IT_OWF, ENABLE);
	  NVIC_EnableIRQ(BACKUP_IRQn);		
}

/* Private functions ------------------------------------- */
void RST_Configuration(void) //HCLK = 48MHz
	
{		
	  RST_CLK_DeInit(); //Set RST_CLK to default
   
  	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE); //Enables the RST_CLK_PCLK_EEPROM
    			
    RST_CLK_HSEconfig(RST_CLK_HSE_ON); //Enable HSE   
    while (RST_CLK_HSEstatus() != SUCCESS) //Good HSE clock
		{
		}
		 
    RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul6); //Select CPU_C1 = HSE
    RST_CLK_CPU_PLLcmd(ENABLE); //Enable PLL 
    while (RST_CLK_CPU_PLLstatus() != SUCCESS) //Good PLL clock
    {      
    }
		
		RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); //Select CPU_C3  =  CPU_C2
    RST_CLK_CPU_PLLuse(ENABLE); //Select CPU_C2 = PLL
    RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3); //Select HCLK =  CPU_C3	

    RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP,ENABLE); //Enables the HSE clock for BKP control		
}
 
void RST_Reconfiguration(void) //HCLK = 8MHz
{
	  RST_CLK_CPU_PLLuse(DISABLE); //Select CPU_C2 = HSE
	  RST_CLK_CPU_PLLcmd(DISABLE); //Dsable PLL    
  	MDR_BKP->REG_0E |= (0 << 0); //Mode of inner DC convertor 
	  MDR_BKP->REG_0E |= (0 << 3); //Value of additional load 	
}

void Pin_Configuration(void)
{ 
	  /* Enables Port A, Port B, Port C, Port D and Port E clocking */ 
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE, ENABLE); 
	
	  PORT_StructInit(&port); //Setting ports initialization structure to the default state
		   
	  /* Configures Pin14 (SCL1) and Pin15 (SDA1) of Port E in Overrid Mode */
	  port.PORT_Pin       = PORT_Pin_14 | PORT_Pin_15; 
	  port.PORT_OE        = PORT_OE_OUT; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_ON; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_OVERRID;
	  port.PORT_SPEED     = PORT_SPEED_MAXFAST;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTE, &port);

    /* Configure PORTE Pin 6 (OSC_In32) and Pin 7 (OSC_Out32) in Analog Mode */
    port.PORT_Pin       = PORT_Pin_6 | PORT_Pin_7;
    port.PORT_OE        = PORT_OE_IN;
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_OUTPUT_OFF;
    port.PORT_MODE      = PORT_MODE_ANALOG;
    PORT_Init(MDR_PORTE, &port);		

    /* Configures Pin6 (Display's Power Switch) and Pin13 (LED) of Port C in Output Driver Mode with Pull Down */
	  port.PORT_Pin       = PORT_Pin_6 | PORT_Pin_13; 
	  port.PORT_OE        = PORT_OE_OUT; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_SPEED_SLOW;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTC, &port);
	
	  PORT_ResetBits(MDR_PORTC, PORT_Pin_6 | PORT_Pin_13);

    /* Configures Pin15 of Port D (Menu Button) in Overrid Mode */
		port.PORT_Pin       = PORT_Pin_15; 
    port.PORT_OE        = PORT_OE_IN; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
		port.PORT_PD_SHM    = PORT_PD_SHM_ON;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_ON;
	  port.PORT_FUNC      = PORT_FUNC_OVERRID;		
	  port.PORT_SPEED     = PORT_SPEED_SLOW;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTD, &port);		

    /* Configure Pin0 (REF+), Pin1 (REF-) and Pin2 (ADC Input) of Port D in Analog Mode */
    port.PORT_Pin       = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2;
    port.PORT_OE        = PORT_OE_IN;
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_OUTPUT_OFF;
    port.PORT_MODE      = PORT_MODE_ANALOG;
    PORT_Init(MDR_PORTD, &port);

    /* Configures Pin7 (Magnetometr Power1) of Port A in Output Driver Mode with Pull Down */
	  port.PORT_Pin       = PORT_Pin_7; 
	  port.PORT_OE        = PORT_OE_OUT; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_SPEED_SLOW;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTA, &port);
	
	  PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
		
		 /* Configures Pin5 (ADC Reference Sourse) and Pin9 (Magnetometr Power2)) of Port B
		 in Output Driver Mode with Pull Down */
	  port.PORT_Pin       = PORT_Pin_5 | PORT_Pin_9; 
	  port.PORT_OE        = PORT_OE_OUT; 
	  port.PORT_PULL_UP   = PORT_PULL_UP_OFF; 
	  port.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	  port.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	  port.PORT_PD        = PORT_PD_DRIVER;
	  port.PORT_GFEN      = PORT_GFEN_OFF;
	  port.PORT_FUNC      = PORT_FUNC_PORT;
	  port.PORT_SPEED     = PORT_SPEED_SLOW;
	  port.PORT_MODE      = PORT_MODE_DIGITAL;
	  PORT_Init(MDR_PORTB, &port);
	
	  PORT_ResetBits(MDR_PORTB, PORT_Pin_5 | PORT_Pin_9);
}   

void I2C_Configuration(void)
{
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_I2C,  ENABLE); //Enables I2C clocking 
	
	  I2C_Cmd(ENABLE); //Enable I2C   
	  
	  /* Configures I2C Initialization Structure */	
	  i2c.I2C_ClkDiv = 0xF; //I2C frequency = 100KHz 
    i2c.I2C_Speed  = I2C_SPEED_UP_TO_400KHz;
    I2C_Init(&i2c);
}

void TIM_Configuration(void)
{
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2 | RST_CLK_PCLK_TIMER3 , ENABLE); //Enables Timer 2 and Timer 3 clocking 
	
	  TIMER_DeInit(MDR_TIMER2);
	  TIMER_DeInit(MDR_TIMER3);
    TIMER_CntStructInit(&timer);	 
	  
	  /* Configures Timer 2 */
	  timer.TIMER_IniCounter       = 1; //
	  timer.TIMER_Prescaler        = 7999; //Timer frequency = 1KHz
	  timer.TIMER_Period           = 2000; //Timer period = 2c
    timer.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;	
	
	  timer.TIMER_CounterDirection = TIMER_CntDir_Up;
	  timer.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;
    	
    TIMER_CntInit (MDR_TIMER2, &timer);
	  TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv1);	
	
	  /* Configures Timer 3 */
	  timer.TIMER_IniCounter       = 1; //
	  timer.TIMER_Prescaler        = 7999; //Timer frequency = 1KHz
	  timer.TIMER_Period           = 4000; //10000; //Timer period = 10c
    timer.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;	
	
	  timer.TIMER_CounterDirection = TIMER_CntDir_Up;
	  timer.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;
    	
    TIMER_CntInit (MDR_TIMER3, &timer);
	  TIMER_BRGInit(MDR_TIMER3, TIMER_HCLKdiv1);	
}




