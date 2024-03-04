 /*
  ******************************************************************************
  * @file    def.h
  * @author  Denis Egorov
  * @version V1.9
  * @date    02/15/2022
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEF_H
#define __DEF_H

/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"                    // Keil::Device:Startup
//#include "MDR32F9Qx_config.h"

/* User define ---------------------------------------------------------------*/
typedef enum
{
  FALSE = 0, TRUE  = !FALSE
} bool;

typedef enum
{
	  ID_REG, DATE_REG, TIME_REG, TPT_REG, MPT_REG, PTR_REG 
} cfg_reg;

typedef enum
{
	  TCNT_REG, MCNT_REG 
} cnt_reg;

typedef enum
{
	  START_FLAG, TMPR_FLAG, MAG_FLAG 
} sys_flag;

struct BCData
{
    uint8_t  ID;          //Device ID
	  uint32_t Date;        //Start date 
    uint16_t Time;        //Start time
	  uint16_t PolTime_T;   //Temperature polling time
	  uint16_t PolTime_M;   //Magnetic field polling time
}; //Basic Configuration Data Structure

struct BMData
{
	  uint8_t  ID;          //Device ID
    uint8_t  Charge_Rate; //Battary lavel
	  int16_t  Hx;          //X-axis magnetic field strength 
	  int16_t  Hy;          //Y-axis magnetic field strength
	  int16_t  Hz;          //Z-axis magnetic field strength
	  uint16_t Hx_var;      //X-axis alternating magnetic field strength
	  uint16_t Hy_var;      //Y-axis alternating magnetic field strength
	  uint16_t Hz_var;      //Z-axis alternating magnetic field strength
	  int16_t  Tmpr;        //Ambient temperature
	  uint16_t Time;        //Real measurement time
	  uint32_t Date;        //Measurement date 
};

struct CalibData
{
    uint16_t T1; //Thermometer compensation word T1
    int16_t  T2; //Thermometer compensation word T2
    int16_t  T3; //Thermometer compensation word T3
    uint8_t SAX; //Magnetometer sensitivity adjustment data for Axis X
    uint8_t SAY; //Magnetometer sensitivity adjustment data for Axis Y
    uint8_t SAZ; //Magnetometer sensitivity adjustment data for Axis Z	
}; //Sensors Adjustment Data Structure 

#define TMPR_MODE      0x00
#define MAG_X_MODE     0x01
#define MAG_Y_MODE     0x02
#define MAG_Z_MODE     0x03
#define MAG_X_VAR_MODE 0x04
#define MAG_Y_VAR_MODE 0x05
#define MAG_Z_VAR_MODE 0x06

#define TMPR_OFF   -200
#define AVR_CFF    100

#define TMPR_FLAG_MSK   ((uint32_t)0x01)
#define MAG_FLAG_MSK    ((uint32_t)0x02)
#define FIRST_START_MSK 0x89ABCDEF
#define SIGN_MSK        0x8000

#define ALARM_VALUE 59

/*Calibration coefficients*/
/*TMPRi = ((TMPRic * TMPR_MUL)/TMPR_DIV) + TMPR_ADD*/
#define TMPR_MUL  1
#define TMPR_DIV  1
#define TMPR_ADD  0

/*MAGCi = ((MAGic * MAG_MUL)/MAG_DIV) + MAG_ADD*/
#define MAG_X_MUL   1
#define MAG_X_DIV   1
#define MAG_X_ADD   0

#define MAG_Y_MUL   1
#define MAG_Y_DIV   1
#define MAG_Y_ADD   0

#define MAG_Z_MUL   1
#define MAG_Z_DIV   1
#define MAG_Z_ADD   0

/*MAGVi = ((MAGVic * MAGV_MUL)/MAGV_DIV) + MAGV_ADD*/
#define MAGV_X_MUL  1
#define MAGV_X_DIV  1
#define MAGV_X_ADD  0

#define MAGV_Y_MUL  1
#define MAGV_Y_DIV  1
#define MAGV_Y_ADD  0

#define MAGV_Z_MUL  1
#define MAGV_Z_DIV  1
#define MAGV_Z_ADD  0

/* Exported constants --------------------------------------------------------*/
static uint8_t start_string[11]    = {0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B}; 
static uint8_t stop_string[11]     = {0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19};
static uint8_t clear_string[11]    = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
static uint8_t zero_string[11]     = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t charge_string[11]   = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1C};
static uint8_t hx_string[11]       = {0x0B, 0x0F, 0x14, 0x1B, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D};
static uint8_t hy_string[11]       = {0x0B, 0x10, 0x14, 0x1B, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D};
static uint8_t hz_string[11]       = {0x0B, 0x11, 0x14, 0x1B, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D};
static uint8_t hx_var_string[11]   = {0x0B, 0x0F, 0x14, 0x15, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D};
static uint8_t hy_var_string[11]   = {0x0B, 0x10, 0x14, 0x15, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D};
static uint8_t hz_var_string[11]   = {0x0B, 0x11, 0x14, 0x15, 0x1B, 0x1B, 0x16, 0x1B, 0x0A, 0x19, 0x0D}; 
static uint8_t tmpr_string[11]     = {0x0E, 0x14, 0x1B, 0x1B, 0x1B, 0x16, 0x1B, 0x0C, 0x1A, 0x1C, 0x1C};
static uint8_t time_string[11]     = {0x1C, 0x1C, 0x1C, 0x00, 0x00, 0x17, 0x00, 0x00, 0x1C, 0x1C, 0x1C};
static uint8_t date_string[11]     = {0x00, 0x00, 0x19, 0x00, 0x00, 0x19, 0x02, 0x00, 0x1B, 0x1B, 0x1C};
static uint8_t t_ic_err_string[11] = {0x1C, 0x00, 0x1B, 0x0E, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
static uint8_t m_ic_err_string[11] = {0x1C, 0x00, 0x1B, 0x0D, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
static uint8_t ic_err_string[11]   = {0x1C, 0x00, 0x1B, 0x0E, 0x19, 0x0D, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};

/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */

#endif 
