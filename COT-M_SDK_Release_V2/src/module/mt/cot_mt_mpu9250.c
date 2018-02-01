
/**
* @file         cot_mt_mpu9250.c
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       wende.wu
* @date         2017-06-09
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/


/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "OSAL.h"
#include "OSAL_Memory.h"
#include "vfs.h"
#include "spi1.h"
#include "error.h"

#include "log.h"
#include "osal_tick.h"


#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "math.h"


#include "cot_mt_mpu9250.h"



#define MPU92_Delay(__TIME)       am_util_delay_ms(__TIME);


#define __USE_MAG_AK8963
#define MAG_READ_DELAY 256

#define ERROR   -1


/* Private macro ---------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------*/


int32_t spi1_fd = 0;  

float aRes, gRes, mRes;      // scale resolutions per LSB for the sensors
int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output
float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer

float gx, gy, gz;
float ax, ay, az;
float mx, my, mz;

int delt_t = 0; // used to control display output rate
int count = 0;  // used to control display output rate

// parameters for 6 DoF sensor fusion calculations
#define  PI  3.14159265358979323846f
#define  GyroMeasError (PI * (60.0f / 180.0f))    // gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
#define  beta (sqrt(3.0f / 4.0f) * GyroMeasError)  // compute beta
#define GyroMeasDrift (PI * (1.0f / 180.0f))     // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
#define  zeta  (sqrt(3.0f / 4.0f) * GyroMeasDrift) // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

float pitch, yaw, roll;
float deltat = 0.0f;                             // integration interval for both filter schemes
int lastUpdate = 0, firstUpdate = 0, Now = 0;    // used to calculate integration interval                               // used to calculate integration interval
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};           // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};              // vector to hold integral error for Mahony method
float sum = 0;
uint32_t sumCount = 0;
int16_t tempCount;   // Stores the real internal chip temperature in degrees Celsius
float temperature;



  
MPU_ConfigTypeDef IMU_InitStruct;



static int32_t get_SPI1_fd( )
{
    return spi1_fd;  
}

static void set_SPI1_fd(int32_t fd)
{
    spi1_fd = fd;
}


int read_us()
{
	return osal_get_systick();
}




/**
  * @brief  MPU92_WriteReg
  * @param  writeAddr: 
  * @param  writeData: 
  * @retval None
  */
void MPU92_WriteReg( uint8_t writeAddr, uint8_t writeData )
{
    ioctl(get_SPI1_fd(),SPI1_CHIPSELECT_MPU9250,TRUE);
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    write(get_SPI1_fd(),&writeAddr,1);  // 
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    write(get_SPI1_fd(),&writeData,1);   
}

/**
  * @brief  MPU92_WriteRegs
  * @param  writeAddr: 
  * @param  writeData: 
  * @param  lens: 
  * @retval None
  */
void MPU92_WriteRegs( uint8_t writeAddr, uint8_t *writeData, uint8_t lens )
{
    ioctl(get_SPI1_fd(),SPI1_CHIPSELECT_MPU9250,TRUE);
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    write(get_SPI1_fd(),&writeAddr,1);  // 
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    write(get_SPI1_fd(),writeData,lens);   
}

/**
  * @brief  MPU92_ReadReg
  * @param  readAddr: 
  * @retval read data
  */
uint8_t MPU92_ReadReg( uint8_t readAddr )
{
    uint8_t readData = 0; 
    uint8_t reg =0;
    ioctl(get_SPI1_fd(),SPI1_CHIPSELECT_MPU9250,TRUE);
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    reg =  0x80 | readAddr;
    write(get_SPI1_fd(),&reg,1);  // 
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    read(get_SPI1_fd(),&readData,1);   
    return readData;
}

/**
  * @brief  MPU92_ReadRegs
  * @param  readAddr: 
  * @param  readData: 
  * @param  lens: 
  * @retval None
  */
void MPU92_ReadRegs( uint8_t readAddr, uint8_t *readData, uint8_t lens )
{
    uint8_t reg =0;
    ioctl(get_SPI1_fd(),SPI1_CHIPSELECT_MPU9250,TRUE);
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    reg =  0x80 | readAddr;
    write(get_SPI1_fd(),&reg,1);  // 
    ioctl(get_SPI1_fd(),SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    read(get_SPI1_fd(),readData,lens);   
}

/**
  * @brief  MPU92_Mag_WriteReg
  * @param  writeAddr: 
  * @param  writeData: 
  * @retval None
  */
void MPU92_Mag_WriteReg( uint8_t writeAddr, uint8_t writeData )
{
    uint8_t  status = 0;
    uint32_t timeout = MAG_READ_DELAY;
    MPU92_WriteReg(MPU6500_I2C_SLV4_ADDR, AK8963_I2C_ADDR);
    MPU92_Delay(1);
    MPU92_WriteReg(MPU6500_I2C_SLV4_REG, writeAddr);
    MPU92_Delay(1);
    MPU92_WriteReg(MPU6500_I2C_SLV4_DO, writeData);
    MPU92_Delay(1);
    MPU92_WriteReg(MPU6500_I2C_SLV4_CTRL, MPU6500_I2C_SLVx_EN);
    MPU92_Delay(1);
    do {
        status = MPU92_ReadReg(MPU6500_I2C_MST_STATUS);
        MPU92_Delay(1);
        if(timeout) (timeout--);
    } while (((status & MPU6500_I2C_SLV4_DONE) == 0) && (timeout));
}

/**
  * @brief  MPU92_Mag_WriteRegs
  * @param  writeAddr: 
  * @param  writeData: 
  * @param  lens: 
  * @retval None
  */
void MPU92_Mag_WriteRegs( uint8_t writeAddr, uint8_t *writeData, uint8_t lens )
{
    uint8_t  status = 0;
    uint32_t timeout = MAG_READ_DELAY;

    MPU92_WriteReg(MPU6500_I2C_SLV4_ADDR, AK8963_I2C_ADDR);
    MPU92_Delay(1);
    for (uint8_t i = 0; i < lens; i++) {
        MPU92_WriteReg(MPU6500_I2C_SLV4_REG, writeAddr + i);
        MPU92_Delay(1);
        MPU92_WriteReg(MPU6500_I2C_SLV4_DO, writeData[i]);
        MPU92_Delay(1);
        MPU92_WriteReg(MPU6500_I2C_SLV4_CTRL, MPU6500_I2C_SLVx_EN);
        MPU92_Delay(1);

        do {
            status = MPU92_ReadReg(MPU6500_I2C_MST_STATUS);
            if(timeout) (timeout--);
        } while (((status & MPU6500_I2C_SLV4_DONE) == 0) && (timeout));
    }
}

/**
  * @brief  MPU92_Mag_ReadReg
  * @param  readAddr: 
  * @retval read data
  */
uint8_t MPU92_Mag_ReadReg( uint8_t readAddr )
{
    uint8_t status = 0;
    uint8_t readData = 0;
    uint32_t timeout = MAG_READ_DELAY;
    MPU92_WriteReg(MPU6500_I2C_SLV4_ADDR, AK8963_I2C_ADDR | 0x80);
    MPU92_Delay(1);
    MPU92_WriteReg(MPU6500_I2C_SLV4_REG, readAddr);
    MPU92_Delay(1);
    MPU92_WriteReg(MPU6500_I2C_SLV4_CTRL, MPU6500_I2C_SLVx_EN);
    MPU92_Delay(1);
    do {
        status = MPU92_ReadReg(MPU6500_I2C_MST_STATUS);
        MPU92_Delay(1);
        if(timeout) (timeout--);
    } while (((status & MPU6500_I2C_SLV4_DONE) == 0) && (timeout));
    readData = MPU92_ReadReg(MPU6500_I2C_SLV4_DI);
    return readData;
}

/**
  * @brief  MPU92_Mag_ReadRegs
  * @param  readAddr: 
  * @param  readData: 
  * @param  lens: 
  * @retval None
  */
void MPU92_Mag_ReadRegs( uint8_t readAddr, uint8_t *readData, uint8_t lens )
{
    uint8_t status = 0;
    uint32_t timeout = MAG_READ_DELAY;

    MPU92_WriteReg(MPU6500_I2C_SLV4_ADDR, AK8963_I2C_ADDR | 0x80);
    MPU92_Delay(1);
    for (uint8_t i = 0; i< lens; i++) {
        MPU92_WriteReg(MPU6500_I2C_SLV4_REG, readAddr + i);
        MPU92_Delay(1);
        MPU92_WriteReg(MPU6500_I2C_SLV4_CTRL, MPU6500_I2C_SLVx_EN);
        MPU92_Delay(1);

        do {
            status = MPU92_ReadReg(MPU6500_I2C_MST_STATUS);
            if(timeout) (timeout--);
        } while (((status & MPU6500_I2C_SLV4_DONE) == 0) && (timeout));

        readData[i] = MPU92_ReadReg(MPU6500_I2C_SLV4_DI);
        MPU92_Delay(1);
    }
}







int8_t MPU92_DeviceCheck( void )
{
    uint8_t deviceID = 0x00;

    deviceID = MPU92_ReadReg(MPU6500_WHO_AM_I);
    if (deviceID != MPU6500_DeviceID) {
        return ERROR;
    }

    #if defined(__USE_MAG_AK8963)
        MPU92_Delay(10);
        deviceID = MPU92_Mag_ReadReg(AK8963_WIA);
        if (deviceID != AK8963_DeviceID) {
            return ERROR;
        }
    #endif
    return SUCCESS;
}



#define MPU6500_InitRegNum  11
#define AK8963_InitRegNum   6

#define MAG_READ_DELAY 256

#define ERROR   -1

int8_t MPU92_Init( MPU_ConfigTypeDef *IMUx,float * destination)
{
    int8_t status = ERROR;
    uint8_t MPU6500_InitData[MPU6500_InitRegNum][2] = {
        {0x80, MPU6500_PWR_MGMT_1},     /* [0]  Reset Device                  */
        {0x04, MPU6500_PWR_MGMT_1},     /* [1]  Clock Source                  */
        {0x10, MPU6500_INT_PIN_CFG},    /* [2]  Set INT_ANYRD_2CLEAR          */
        {0x01, MPU6500_INT_ENABLE},     /* [3]  Set RAW_RDY_EN                */
        {0x00, MPU6500_PWR_MGMT_2},     /* [4]  Enable Acc & Gyro             */
        {0x00, MPU6500_SMPLRT_DIV},     /* [5]  Sample Rate Divider           */
        {0x00, MPU6500_GYRO_CONFIG},    /* [6]  default : +-250dps            */
        {0x00, MPU6500_ACCEL_CONFIG},   /* [7]  default : +-2G                */
        {0x00, MPU6500_CONFIG},         /* [8]  default : GyrLPS_250Hz        */
        {0x00, MPU6500_ACCEL_CONFIG_2}, /* [9]  default : AccLPS_460Hz        */
        {0x30, MPU6500_USER_CTRL},      /* [10] Set I2C_MST_EN, I2C_IF_DIS    */
    };
    uint8_t AK8963_InitData[AK8963_InitRegNum][2] = {
        {0x01, AK8963_CNTL2},           /* [0]  Reset Device                  */
        {0x00, AK8963_CNTL1},           /* [1]  Power-down mode               */
        {0x0F, AK8963_CNTL1},           /* [2]  Fuse ROM access mode          */
        {0x00, AK8963_ASAX},            /* [3]  Read sensitivity adjustment   */
        {0x00, AK8963_CNTL1},           /* [4]  Power-down mode               */
        {0x06, AK8963_CNTL1},           /* [5]  Continuous measurement mode 2 */
    };

    MPU6500_InitData[6][0] = IMUx->MPU_Gyr_FullScale;       /* [6] MPU6500_GYRO_CONFIG */
    MPU6500_InitData[7][0] = IMUx->MPU_Acc_FullScale;       /* [7] MPU6500_ACCEL_CONFIG */
    MPU6500_InitData[8][0] = IMUx->MPU_Gyr_LowPassFilter;   /* [8] MPU6500_CONFIG */
    MPU6500_InitData[9][0] = IMUx->MPU_Acc_LowPassFilter;   /* [9] MPU6500_ACCEL_CONFIG_2 */

    for (uint32_t i = 1; i < MPU6500_InitRegNum; i++) // ÒÑ¾­ÖØÆô for (uint32_t i = 0; i < MPU6500_InitRegNum; i++) 
    {
        MPU92_Delay(2);
        MPU92_WriteReg(MPU6500_InitData[i][1], MPU6500_InitData[i][0]);
    }

    MPU92_Delay(2);
    status = MPU92_DeviceCheck();
    if (status != SUCCESS)
        return ERROR;

#if defined(__USE_MAG_AK8963)
    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
    AK8963_InitData[5][0] |= IMUx->MPU_Mag_FullScale | IMUx->MPU_Mag_Mmode;        /* [4] AK8963_CNTL1 */

    MPU92_Delay(2);
    MPU92_Mag_WriteReg(AK8963_InitData[0][1], AK8963_InitData[0][0]);
    MPU92_Delay(2);
    MPU92_Mag_WriteReg(AK8963_InitData[1][1], AK8963_InitData[1][0]);
    MPU92_Delay(2);
    MPU92_Mag_WriteReg(AK8963_InitData[2][1], AK8963_InitData[2][0]);
    MPU92_Delay(2);
    {        
        uint8_t rawData[3]; 
        MPU92_ReadRegs(AK8963_ASAX, &rawData[0], 3);  // Read the x-, y-, and z-axis calibration values
        destination[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return x-axis sensitivity adjustment values, etc.
        destination[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
        destination[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
        MPU92_Delay(2);
    }
    MPU92_Mag_WriteReg(AK8963_InitData[4][1], AK8963_InitData[4][0]);
    MPU92_Delay(2);
    MPU92_Mag_WriteReg(AK8963_InitData[5][1], AK8963_InitData[5][0]);

    /* config mpu9250 i2c */
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_MST_CTRL, 0x5D);
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_SLV0_ADDR, AK8963_I2C_ADDR | 0x80);
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_SLV0_REG, AK8963_ST1);
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_SLV0_CTRL, MPU6500_I2C_SLVx_EN | 8);
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_SLV4_CTRL, 0x09);
    MPU92_Delay(2);
    MPU92_WriteReg(MPU6500_I2C_MST_DELAY_CTRL, 0x81);
#endif
    MPU92_Delay(100);
    return SUCCESS;
}






/**
  * @brief  MPU92_GetRawData
  * @param  data: point to int16_t
  * @retval return 1 : AK8963 data update
            return 0 : AK8963 data not update
  */
int8_t MPU92_GetRawData( int16_t *data )
{
#if defined(__USE_MAG_AK8963)
    uint8_t readBuf[22] = {0};
    MPU92_ReadRegs(MPU6500_ACCEL_XOUT_H, readBuf, 22);    /* Read Gyr, Acc, Mag */
#else
    uint8_t readBuf[14] = {0};
    MPU92_ReadRegs(MPU6500_ACCEL_XOUT_H, readBuf, 14);    /* Read Gyr, Acc */
#endif

    data[0] = (int16_t)(readBuf[8]  << 8) | readBuf[9];   /* Gyr.X */
    data[1] = (int16_t)(readBuf[10] << 8) | readBuf[11];  /* Gyr.Y */
    data[2] = (int16_t)(readBuf[12] << 8) | readBuf[13];  /* Gyr.Z */
    data[3] = (int16_t)(readBuf[0]  << 8) | readBuf[1];   /* Acc.X */
    data[4] = (int16_t)(readBuf[2]  << 8) | readBuf[3];   /* Acc.Y */
    data[5] = (int16_t)(readBuf[4]  << 8) | readBuf[5];   /* Acc.Z */
    data[6] = (int16_t)(readBuf[6]  << 8) | readBuf[7];   /* ICTemp */

#if defined(__USE_MAG_AK8963)
    if (!(!(readBuf[14] & AK8963_STATUS_DRDY) || (readBuf[14] & AK8963_STATUS_DOR) || (readBuf[21] & AK8963_STATUS_HOFL))) {
        data[7] = (int16_t)(readBuf[16] << 8) | readBuf[15];  /* Mag.X */
        data[8] = (int16_t)(readBuf[18] << 8) | readBuf[17];  /* Mag.Y */
        data[9] = (int16_t)(readBuf[20] << 8) | readBuf[19];  /* Mag.Z */
        return SUCCESS;
    }
    else
        return ERROR;
#else
    return SUCCESS;
#endif
}



//===================================================================================================================
//====== Set of useful function to access acceleratio, gyroscope, and temperature data
//===================================================================================================================
void getMres() 
{
  switch (IMU_InitStruct.MPU_Mag_FullScale)
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    case MPU_MagFS_14b:
          mRes = 10.0*4912.0/8190.0; // Proper scale to return milliGauss
          break;
    case MPU_MagFS_16b:
          mRes = 10.0*4912.0/32760.0; // Proper scale to return milliGauss
          break;
  }
}

void getGres() 
{
  switch (IMU_InitStruct.MPU_Gyr_FullScale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case MPU_GyrFS_250dps:
          gRes = 250.0/32768.0;
          break;
    case MPU_GyrFS_500dps:
          gRes = 500.0/32768.0;
          break;
    case MPU_GyrFS_1000dps:
          gRes = 1000.0/32768.0;
          break;
    case MPU_GyrFS_2000dps:
          gRes = 2000.0/32768.0;
          break;
  }
}


void getAres() 
{
  switch (IMU_InitStruct.MPU_Acc_FullScale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case MPU_AccFS_2g:
          aRes = 2.0/32768.0;
          break;
    case MPU_AccFS_4g:
          aRes = 4.0/32768.0;
          break;
    case MPU_AccFS_8g:
          aRes = 8.0/32768.0;
          break;
    case MPU_AccFS_16g:
          aRes = 16.0/32768.0;
          break;
  }
}


void resetMPU9250() 
{
    // reset device
    MPU92_WriteReg(MPU6500_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    MPU92_Delay(100);
}


// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void calibrateMPU9250(float * dest1, float * dest2)
{
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count, fifo_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

    // reset device, reset all registers, clear gyro and accelerometer bias registers
    MPU92_WriteReg(MPU6500_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    MPU92_Delay(100);

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    MPU92_WriteReg(MPU6500_PWR_MGMT_1, 0x01);
    MPU92_WriteReg(MPU6500_PWR_MGMT_2, 0x00);
    MPU92_Delay(200);

    // Configure device for bias calculation
    MPU92_WriteReg(MPU6500_INT_ENABLE, 0x00);   // Disable all interrupts
    MPU92_WriteReg(MPU6500_FIFO_EN, 0x00);      // Disable FIFO
    MPU92_WriteReg(MPU6500_PWR_MGMT_1, 0x00);   // Turn on internal clock source
    MPU92_WriteReg(MPU6500_I2C_MST_CTRL, 0x00); // Disable I2C master
    MPU92_WriteReg(MPU6500_USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    MPU92_WriteReg(MPU6500_USER_CTRL, 0x0C);    // Reset FIFO and DMP
    MPU92_Delay(15);

    // Configure MPU9250 gyro and accelerometer for bias calculation
    MPU92_WriteReg(MPU6500_CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    MPU92_WriteReg(MPU6500_SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    MPU92_WriteReg(MPU6500_GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    MPU92_WriteReg(MPU6500_ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    MPU92_WriteReg(MPU6500_USER_CTRL, 0x40);   // Enable FIFO
    MPU92_WriteReg(MPU6500_FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    MPU92_Delay(40); // accumulate 40 samples in 80 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    MPU92_WriteReg(MPU6500_FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    MPU92_Mag_ReadRegs(MPU6500_FIFO_COUNTH, &data[0], 2); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (ii = 0; ii < packet_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    MPU92_Mag_ReadRegs(MPU6500_FIFO_R_W, &data[0], 12); // read data for averaging
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
    gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];

    }
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
    else {accel_bias[2] += (int32_t) accelsensitivity;}

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    /// Push gyro biases to hardware registers
    /*  writeByte(MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
    writeByte(MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
    writeByte(MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
    writeByte(MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
    writeByte(MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
    writeByte(MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);
    */
    dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
    dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    MPU92_Mag_ReadRegs(MPU6500_XA_OFFSET_H, &data[0], 2); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    MPU92_Mag_ReadRegs(MPU6500_YA_OFFSET_H, &data[0], 2);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    MPU92_Mag_ReadRegs(MPU6500_ZA_OFFSET_H, &data[0], 2);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(ii = 0; ii < 3; ii++) {
    if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
    /*  writeByte(MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
    writeByte(MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
    writeByte(MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
    writeByte(MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
    writeByte(MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
    writeByte(MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);
    */
    // Output scaled accelerometer biases for manual subtraction in the main program
    dest2[0] = (float)accel_bias[0]/(float)accelsensitivity;
    dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
    dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}


// Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
// measured ones.
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, bx, bz;
    float vx, vy, vz, wx, wy, wz;
    float ex, ey, ez;
    float pa, pb, pc;

    // Auxiliary variables to avoid repeated arithmetic
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f / norm;        // use reciprocal for division
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f / norm;        // use reciprocal for division
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
    hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
    bx = sqrt((hx * hx) + (hy * hy));
    bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

    // Estimated direction of gravity and magnetic field
    vx = 2.0f * (q2q4 - q1q3);
    vy = 2.0f * (q1q2 + q3q4);
    vz = q1q1 - q2q2 - q3q3 + q4q4;
    wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
    wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
    wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);

    // Error is cross product between estimated direction and measured direction of gravity
    ex = (ay * vz - az * vy) + (my * wz - mz * wy);
    ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
    ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
    if (Ki > 0.0f)
    {
        eInt[0] += ex;      // accumulate integral error
        eInt[1] += ey;
        eInt[2] += ez;
    }
    else
    {
        eInt[0] = 0.0f;     // prevent integral wind up
        eInt[1] = 0.0f;
        eInt[2] = 0.0f;
    }

    // Apply feedback terms
    gx = gx + Kp * ex + Ki * eInt[0];
    gy = gy + Kp * ey + Ki * eInt[1];
    gz = gz + Kp * ez + Ki * eInt[2];

    // Integrate rate of change of quaternion
    pa = q2;
    pb = q3;
    pc = q4;
    q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
    q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
    q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
    q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);

    // Normalise quaternion
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
    norm = 1.0f / norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;

}

int16_t readTempData()
{
  uint8_t rawData[2];  // x/y/z gyro register data stored here
  MPU92_ReadRegs(MPU6500_TEMP_OUT_H, &rawData[0], 2);  // Read the two raw data registers sequentially into data array
  return (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]) ;  // Turn the MSB and LSB into a 16-bit value
}



void handleMPU9250date()
{
    int p,r,y;
    
    Now = read_us();
    deltat = (float)((Now - lastUpdate)/1000.0f) ; // set integration time by time elapsed since last filter update
    lastUpdate = Now;

    sum += deltat;
    sumCount++;


    // Serial print and/or display at 0.5 s rate independent of data rates
    delt_t = read_us() - count;
    if (delt_t > 500) 
    { // 
        
        MahonyQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, my, mx, mz);

        log_printf("ax = %f", 1000*ax);
        log_printf(" ay = %f", 1000*ay);
        log_printf(" az = %f  mg\n\r", 1000*az);

        log_printf("gx = %f", gx);
        log_printf(" gy = %f", gy);
        log_printf(" gz = %f  deg/s\n\r", gz);

        log_printf("gx = %f", mx);
        log_printf(" gy = %f", my);
        log_printf(" gz = %f  mG\n\r", mz);

        //tempCount = readTempData();  // Read the adc values
        //temperature = ((float) tempCount) / 333.87f + 21.0f; // Temperature in degrees Centigrade
        //log_printf(" temperature = %f  C\n\r", temperature);

        log_printf("q0 = %f\n\r", q[0]);
        log_printf("q1 = %f\n\r", q[1]);
        log_printf("q2 = %f\n\r", q[2]);
        log_printf("q3 = %f\n\r", q[3]);



      // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
      // In this coordinate system, the positive z-axis is down toward Earth.
      // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
      // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
      // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
      // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
      // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
      // applied in the correct order which for this configuration is yaw, pitch, and then roll.
      // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
        yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
        pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
        roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
        pitch *= 180.0f / PI;
        yaw   *= 180.0f / PI;
        //yaw   -= 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
        roll  *= 180.0f / PI;

    	p = pitch;
    	y = yaw;
    	r = roll;

    	if(r > 0)
    		r -=180;
    	else if(r < 0)
    		r += 180;

    	y = y  -r;

        log_printf("Yaw, Pitch, Roll: %f %f %f\n\r", yaw, pitch, roll);

        log_printf("Yaw, Pitch, Roll: %d %d %d\n\r", y, p, r);
        log_printf("average rate = %f\n\r", (float) sumCount/sum);

        count = read_us();
        sum = 0;
        sumCount = 0;
    }

    
}


/**
 * @name: cot_mt_mpu9250_GetTemp
 * This function provide simple useing of the mpu9250 Get Temp.
 * @param[in]   none.
 * @param[out]  *temp  temperature .
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by wende.wu on 2017-06-19
 */
int8_t cot_mt_mpu9250_GetTemp(float *temp)
{
    int32_t spi1_fd = 0; 
    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )
        return ERR_ERROR;
    set_SPI1_fd(spi1_fd);
    
    tempCount = readTempData();  // Read the adc values
    *temp = temperature = ((float) tempCount) / 333.87f + 21.0f; // Temperature in degrees Centigrade
    log_printf(" temperature = %f  C\n\r", temperature);
    
    
    close(spi1_fd);
    return ERR_SUCCESS;
}

/**
 * @name: cot_mt_mpu9250_GetRaw
 * This function provide simple useing of the mpu9250 Get Raw.
 * @param[in]   none.
 * @param[out]  gyr[]  gyr.x  gyr.y   gyr.z .
                acc[]  acc.x  acc.y   acc.z .
                mag[]  mag.x  mag.y   mag.z .
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by wende.wu on 2017-06-19
 */
int8_t cot_mt_mpu9250_GetRaw(int16_t gyr[],int16_t acc[],int16_t mag[])
{
    int32_t ret = ERR_SUCCESS;
    int32_t spi1_fd = 0; 
    int16_t data[10];
    
    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )
        return ERR_ERROR;
    set_SPI1_fd(spi1_fd);
    
    
    // If intPin goes high, all data registers have new data
    if(MPU92_ReadReg(MPU6500_INT_STATUS) & 0x01) 
    {  // On interrupt, check if data ready interrupt

        ret = MPU92_GetRawData(data);
        
        log_printf("ret = %d\n",ret );
        
        log_printf("Gyr.X = %d\n",data[0]);
        log_printf("Gyr.Y = %d\n",data[1] );
        log_printf("Gyr.Z = %d\n",data[2] );
        gyr[0] = gyroCount[0] = data[0];
        gyr[1] = gyroCount[1] = data[1];
        gyr[2] = gyroCount[2] = data[2];
            
        log_printf("Acc.X = %d\n",data[3]);
        log_printf("Acc.Y = %d\n",data[4] );
        log_printf("Acc.Z = %d\n",data[5] );
        acc[0] = accelCount[0] = data[3];
        acc[1] = accelCount[1] = data[4];
        acc[2] = accelCount[2] = data[5];
        
        log_printf("ICTemp = %d\n",data[6] );
        
        log_printf("Mag.X = %d\n",data[7]);
        log_printf("Mag.Y = %d\n",data[8] );
        log_printf("Mag.Z = %d\n",data[9] );
        mag[0] = magCount[0] = data[7];
        mag[1] = magCount[1] = data[8];
        mag[2] = magCount[2] = data[9];
        
        // Now we'll calculate the accleration value into actual g's
        ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
        ay = (float)accelCount[1]*aRes - accelBias[1];
        az = (float)accelCount[2]*aRes - accelBias[2];
        
        // Calculate the gyro value into actual degrees per second
        gx = (float)gyroCount[0]*gRes;// - gyroBias[0];  // get actual gyro value, this depends on scale being set
        gy = (float)gyroCount[1]*gRes;// - gyroBias[1];
        gz = (float)gyroCount[2]*gRes;// - gyroBias[2];
        
        // Calculate the magnetometer values in milliGauss
        // Include factory calibration per data sheet and user environmental corrections
        mx = (float)magCount[0]*mRes*magCalibration[0];// - magbias[0];  // get actual magnetometer value, this depends on scale being set
        my = (float)magCount[1]*mRes*magCalibration[1];// - magbias[1];
        mz = (float)magCount[2]*mRes*magCalibration[2];// - magbias[2];

    }
    else
        ret = ERR_FAILUER;
    
    close(spi1_fd);
    return ret;
}

/**
 * @name: cot_mt_mpu9250_init
 * This function provide simple useing of the mpu9250 init.
 * @param[in]   none.
 * @param[out]  none.
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by wende.wu on 2017-06-19
    INT     GPIO24
    CS	    GPIO14
    SCLK	GPIO8
    SDI	    GPIO10
    SDO	    GPIO9
 */
int32_t cot_mt_mpu9250_init( void )
{
    int32_t ret = 0;
    int32_t spi1_fd = 0;  

    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )
        return ERR_ERROR;
    set_SPI1_fd(spi1_fd);

    resetMPU9250(); // Reset registers to default in preparation for device calibration
    calibrateMPU9250(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
    log_printf("x gyro bias = %f\n\r", gyroBias[0]);
    log_printf("y gyro bias = %f\n\r", gyroBias[1]);
    log_printf("z gyro bias = %f\n\r", gyroBias[2]);
    log_printf("x accel bias = %f\n\r", accelBias[0]);
    log_printf("y accel bias = %f\n\r", accelBias[1]);
    log_printf("z accel bias = %f\n\r", accelBias[2]);  
    
    IMU_InitStruct.MPU_Gyr_FullScale      = MPU_GyrFS_2000dps;
    IMU_InitStruct.MPU_Gyr_LowPassFilter  = MPU_GyrLPS_41Hz;
    IMU_InitStruct.MPU_Acc_FullScale      = MPU_AccFS_4g;
    IMU_InitStruct.MPU_Acc_LowPassFilter  = MPU_AccLPS_41Hz;
    IMU_InitStruct.MPU_Mag_FullScale      = MPU_MagFS_16b;
    IMU_InitStruct.MPU_Mag_Mmode          = 0x06;        // Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR
    
    MPU92_Init(&IMU_InitStruct,magCalibration);


    
    getAres(); // Get accelerometer sensitivity
    getGres(); // Get gyro sensitivity
    getMres(); // Get magnetometer sensitivity
    log_printf("Accelerometer sensitivity is %f LSB/g \n\r", 1.0f/aRes);
    log_printf("Gyroscope sensitivity is %f LSB/deg/s \n\r", 1.0f/gRes);
    log_printf("Magnetometer sensitivity is %f LSB/G \n\r", 1.0f/mRes);
    magbias[0] = +470.;  // User environmental x-axis correction in milliGauss, should be automatically calculated
    magbias[1] = +120.;  // User environmental x-axis correction in milliGauss
    magbias[2] = +125.;  // User environmental x-axis correction in milliGauss

    close(spi1_fd);
    return ret;
}



