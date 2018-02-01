/**
* @file         cot_mt_bmp280.c
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
#include "cot_mt_bmp280.h"
#include "log.h"


#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

#define BMP280_CHIPID   0x58

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
    



typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} bmp280_calib_data;

static bmp280_calib_data bmp280_calib;




/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/
/* cot application protocal parameters */


/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over SPI
*/
/**************************************************************************/
void write8(int32_t spi1_fd,uint8_t reg, uint8_t value)
{
    ioctl(spi1_fd,SPI1_CHIPSELECT_BMP280,TRUE);
    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    reg = (reg & ~0x80); // write, bit 7 low
    write(spi1_fd,&reg,1);  // 
    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    write(spi1_fd,&value,1);  // 

}



/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over SPI
*/
/**************************************************************************/
uint8_t read8(int32_t spi1_fd,uint8_t reg)
{
    uint8_t  temp = 0;
    
    ioctl(spi1_fd,SPI1_CHIPSELECT_BMP280,TRUE);
    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    reg = (reg | 0x80);// read, bit 7 high
    write(spi1_fd,&reg,1);  // 
    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    read(spi1_fd,&temp,1);
    
    return (uint8_t)temp;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over SPI
*/
/**************************************************************************/
uint16_t read16(int32_t spi1_fd,uint8_t reg)
{
    uint16_t  temp = 0;
    
    ioctl(spi1_fd,SPI1_CHIPSELECT_BMP280,TRUE);
    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_LOW);
    reg = (reg | 0x80);// read, bit 7 high
    write(spi1_fd,&reg,1);

    ioctl(spi1_fd,SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS,SPI1_SET_NSS_HIGH);
    read(spi1_fd,&temp,2);

    temp = (temp << 8)|(temp >> 8);
    
    return (uint16_t)temp;
}

uint16_t read16_LE(int32_t spi1_fd,uint8_t reg) {
  uint16_t temp = read16(spi1_fd,reg);
  return (temp >> 8) | (temp << 8);
  
}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over SPI
*/
/**************************************************************************/
int16_t readS16(int32_t spi1_fd,uint8_t reg)
{
  return (int16_t)read16(spi1_fd,reg);

}

int16_t readS16_LE(int32_t spi1_fd,uint8_t reg)
{
  return (int16_t)read16_LE(spi1_fd,reg);

}


/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void readCoefficients(int32_t spi1_fd)
{
    bmp280_calib.dig_T1 = read16_LE(spi1_fd,BMP280_REGISTER_DIG_T1);
    bmp280_calib.dig_T2 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_T2);
    bmp280_calib.dig_T3 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_T3);

    bmp280_calib.dig_P1 = read16_LE(spi1_fd,BMP280_REGISTER_DIG_P1);
    bmp280_calib.dig_P2 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P2);
    bmp280_calib.dig_P3 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P3);
    bmp280_calib.dig_P4 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P4);
    bmp280_calib.dig_P5 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P5);
    bmp280_calib.dig_P6 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P6);
    bmp280_calib.dig_P7 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P7);
    bmp280_calib.dig_P8 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P8);
    bmp280_calib.dig_P9 = readS16_LE(spi1_fd,BMP280_REGISTER_DIG_P9);
}






int32_t t_fine = 0;

/**
* @name: bmp280_read_temperature
* This funtion deal with the bmp280 read temperature
* @param[in]   none.
* @param[out]   temperature temperature
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
int8_t bmp280_read_temperature(float *tem)
{
    int32_t var1, var2;
    int32_t adc_T = 0;
    int32_t spi1_fd = 0;  
    
    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )  return ERR_ERROR;
    
    adc_T = read16(spi1_fd,BMP280_REGISTER_TEMPDATA);
    adc_T <<= 8;
    adc_T |= read8(spi1_fd,BMP280_REGISTER_TEMPDATA+2);
    adc_T >>= 4;

    var1  = ((((adc_T>>3) - ((int32_t)bmp280_calib.dig_T1 <<1))) * \
            ((int32_t)bmp280_calib.dig_T2)) >> 11;

    var2  = (((((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1)) * \
            ((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1))) >> 12) * \
            ((int32_t)bmp280_calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T  = (t_fine * 5 + 128) >> 8;

    close(spi1_fd);
    *tem = T/100;
    return ERR_SUCCESS;
}

/**
* @name: bmp280_read_pressure
* This funtion deal with the bmp280 read pressure
* @param[in]   none.
* @param[out]   temperature temperature
                pressure    pressure
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
int8_t bmp280_read_pressure(float *temperature,float *pressure) 
{
    int64_t var1, var2, p;
    int32_t adc_P = 0;
    int32_t spi1_fd = 0;  

    bmp280_read_temperature(temperature); //  read t_fine
     
    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )  return ERR_ERROR;

    adc_P = read16(spi1_fd,BMP280_REGISTER_PRESSUREDATA);
    adc_P <<= 8;
    adc_P |= read8(spi1_fd,BMP280_REGISTER_PRESSUREDATA+2);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)bmp280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)bmp280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)bmp280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)bmp280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bmp280_calib.dig_P1)>>33;

    if (var1 == 0) {
        close(spi1_fd);
        return ERR_ERROR;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_calib.dig_P7)<<4);

    close(spi1_fd);
    *pressure = (float)p/256;
    return ERR_SUCCESS;
}


/**
* @name: cot_mt_bmp280_init
* This funtion deal with the bmp280 init
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
CS      GPIO22
SCLK    GPIO8
SDI     GPIO10
SDO     GPIO9
*/
int32_t cot_mt_bmp280_init(void)
{
    int32_t ret = 0;
    int32_t spi1_fd = 0;  
    
    spi1_fd = open(PATH_SPI1,0);
    if( spi1_fd <= 0 )  return ERR_ERROR;

    if (read8(spi1_fd,BMP280_REGISTER_CHIPID) != BMP280_CHIPID)
        ret = 1;
    readCoefficients(spi1_fd);
    write8(spi1_fd,BMP280_REGISTER_CONTROL, 0x3F);
    close(spi1_fd);
    
    #if 0
    {
        uint8_t i = 0;
        float temperature = 0;
        float pressure = 0;

        for(i = 0;i < 0xff; i++)
        {
            bmp280_read_pressure(&temperature,&pressure);
            log_printf("\n temperature = %f \n",temperature);
            log_printf(" pressure = %f \n",pressure);
            am_util_delay_ms(2000);
        }
    }
    #endif
    
    
    return ret;
}

