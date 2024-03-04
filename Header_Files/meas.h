/*
  ******************************************************************************
  * @file    meas.h
  * @author  Denis Egorov
  * @version V1.12
  * @date    12/11/2020 
  * @brief   
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEAS_H
#define __MEAS_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "MDR32F9Qx_i2c.h"              // Keil::Drivers:I2C

/* User define ---------------------------------------------------------------*/
#define BMP_ADDR         0xEE //Address of the temperature sensor's IC
#define MPU_ADDR         0xD0 //Address of the magnetic field sensor's IC
#define MAG_ADDR         0x18 //0x0C Address of the magnetometer
#define BMP_ID_ADDR      0xD0 //Address of ID register of the temperature sensor's IC 
#define MPU_ID_ADDR      0x75 //Address of ID register of the magnetic field sensor's IC 
#define BMP_T1_LSB_ADDR  0x88 //Address of the thermometer compensation coefficient T1 register (LSB)
#define BMP_T1_MSB_ADDR  0x89 //Address of the thermometer compensation coefficient T1 register (MSB) 
#define BMP_T2_LSB_ADDR  0x8A //Address of the thermometer compensation coefficient T2 register (LSB)
#define BMP_T2_MSB_ADDR  0x8B //Address of the thermometer compensation coefficient T2 register (MSB)
#define BMP_T3_LSB_ADDR  0x8C //Address of the thermometer compensation coefficient T3 register (LSB)
#define BMP_T3_MSB_ADDR  0x8D //Address of the thermometer compensation coefficient T3 register (MSB)
#define BMP_CONTROL_ADDR 0xF4 //Address of control register of temperature sensor's IC 
#define BMP_CONFIG_ADDR  0xF5 //Address of configuration register of temperature sensor's IC
#define BMP_T_MSB_ADDR   0xFA //Address of the temperature register (MSB)
#define BMP_T_LSB_ADDR   0xFB //Address of the temperature register (LSB)
#define	PWR_MGMT_1_ADDR	 0x6B	//Address of the power management register. Typical values:0x00(run mode)
#define	SMPLRT_DIV_ADDR	 0x19	//Address of the sample rate divider register. Typical values:0x07(125Hz),1KHz internal sample rate
#define	CONFIG_ADDR			 0x1A	//Address of the low pass filter register. Typical values:0x06(5Hz)
#define	INT_BYPASS_ADDR	 0x37	//Address of the intertupt/bypass register
#define	ASA_X_ADDR	     0x10	//Address of the magnetic sensor X-axis sensitivity adjustment register
#define	ASA_Y_ADDR	     0x11	//Address of the magnetic sensor Y-axis sensitivity adjustment register
#define	ASA_Z_ADDR	     0x12	//Address of the magnetic sensor Z-axis sensitivity adjustment register
#define MPU_X_LSB_ADDR   0x03 //Address of the magnetic field strength register X (LSB)
#define MPU_X_MSB_ADDR   0x04 //Address of the magnetic field strength register X( MSB) 
#define MPU_Y_LSB_ADDR   0x05 //Address of the magnetic field strength register Y (LSB)
#define MPU_Y_MSB_ADDR   0x06 //Address of the magnetic field strength register Y (MSB)
#define MPU_Z_LSB_ADDR   0x07 //Address of the magnetic field strength register Z (LSB)
#define MPU_Z_MSB_ADDR   0x08 //Address of the magnetic field strength register Z ( MSB)
#define	CNTL_1_ADDR	     0x0A	//Address of the magnetic sensor IC control 1 register
#define	CNTL_2_ADDR	     0x0B	//Address of the magnetic sensor IC control 2 register
#define	ST_1_ADDR	       0x02	//Address of the magnetic sensor IC status 1 register
#define	ST_2_ADDR	       0x09	//Address of the magnetic sensor IC status 2 register
#define	ASTC_ADDR	       0x0C	//Address of the magnetic sensor IC automatic self test control register
#define BMP180_ID        0x55 //Value of ID BMP180
#define BMP280_ID        0x58 //Value of ID BMP280
#define MPU9250_ID       0x71 //Value of ID MPU9250
#define MPU9255_ID       0x73 //Value of ID MPU9255
#define INIT_VALUE_TMPR  0x00 //Initialization value of the BPM IC
#define SLEEP_MODE_TMPR  0x20 //Sleep mode value of the BPM IC 
#define FORSED_MODE_TMPR 0x22 //Forsed mode value of the BPM IC

/* Variables -----------------------------------------------------------------*/
 																		
/* Exported functions prototypes -------------------------------------------- */
bool CheckSensorsInterface(uint8_t* Str_Array); //Checking communication with sensors
void ConfigTmprSensor(void); //Configuration the temperature sensor's IC 
void ReadTmprCompData(struct CalibData* Factor); //Reading the termometer compensation data
void ReadTmprData(uint16_t* MeasValue); //Reading the current temperature measurement data
int16_t GetActualTmpr(uint16_t* MeasValue, struct CalibData Factor); //Calculating the actual temperature
void ConfigMagSensor(void); //Configuration the magnetic sensor's IC 
void ReadMagAdjData(struct CalibData* Factor); //Reading the magnetometer sensitivity adjustment data
void ReadMagData(uint16_t* MeasValue, uint8_t AvrNum); //Reading the current magnetometer measurement data
void GetMagStrength(uint16_t* MeasValue, struct BMData* Data); //Calculating the magnetic field strength
void MagSelfTest(uint16_t* MeasValue); //Self test of the magnetometer

#endif 
