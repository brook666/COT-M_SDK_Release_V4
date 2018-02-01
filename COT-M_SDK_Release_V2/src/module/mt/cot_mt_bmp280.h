/**
* @file         cot_mt_bmp280.h
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       wende.wu
* @date         2017-06-09
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_MT_BMP280_H
#define COT_MT_BMP280_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                               INCLUDES
 ***************************************************************************************************/


/***************************************************************************************************
 * CONST
 ***************************************************************************************************/

       
/*=========================================================================
REGISTERS
-----------------------------------------------------------------------*/
enum
{
    BMP280_REGISTER_DIG_T1              = 0x88,
    BMP280_REGISTER_DIG_T2              = 0x8A,
    BMP280_REGISTER_DIG_T3              = 0x8C,

    BMP280_REGISTER_DIG_P1              = 0x8E,
    BMP280_REGISTER_DIG_P2              = 0x90,
    BMP280_REGISTER_DIG_P3              = 0x92,
    BMP280_REGISTER_DIG_P4              = 0x94,
    BMP280_REGISTER_DIG_P5              = 0x96,
    BMP280_REGISTER_DIG_P6              = 0x98,
    BMP280_REGISTER_DIG_P7              = 0x9A,
    BMP280_REGISTER_DIG_P8              = 0x9C,
    BMP280_REGISTER_DIG_P9              = 0x9E,

    BMP280_REGISTER_CHIPID             = 0xD0,
    BMP280_REGISTER_VERSION            = 0xD1,
    BMP280_REGISTER_SOFTRESET          = 0xE0,

    BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

    BMP280_REGISTER_CONTROL            = 0xF4,
    BMP280_REGISTER_CONFIG             = 0xF5,
    BMP280_REGISTER_PRESSUREDATA       = 0xF7,
    BMP280_REGISTER_TEMPDATA           = 0xFA,
};

/*=========================================================================*/



/***************************************************************************************************
 * EXTERNALS
 ***************************************************************************************************/

/***************************************************************************************************
 * FUNCTIONS
 ***************************************************************************************************/


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
int8_t bmp280_read_temperature(float *tem);


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
int8_t bmp280_read_pressure(float *temperature,float *pressure);


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
int32_t cot_mt_bmp280_init(void);


#ifdef __cplusplus
}
#endif

#endif  /* COT_MT_BMP9250_H */

