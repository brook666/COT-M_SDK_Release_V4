/**
* @file         cot_mt_oled.h
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       wende.wu
* @date         2017-06-01
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_MT_OLED_H
#define COT_MT_OLED_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                               INCLUDES
 ***************************************************************************************************/
#include <stdint.h>




/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

#define   OLED_SHOWLIGHT    1
#define   OLED_SHOWDARK     0


/** 
* @name: cot_mt_oled_init 
* This function deal with the oled init.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
int32_t cot_mt_oled_init(void);

/** 
* @name: OLED_CLS 
* This function handles oled clear the display.
* @param[in]    none
* @param[out]   none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_CLS(void);//ÇåÆÁ

void oled_line_cls( uint8_t m ); 

/** 
* @name: OLED_Fill 
* This function handles oled display lattice.
* @param[in]    lattice  lattice
* @param[out]   none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_Fill(uint8_t lattice);//È«ÆÁÌî³ä



/** 
* @name: OLED_ShowStr6x8 
* This function deal with the oled Show asc String.
* @param[in]   shiny shiny
               x    coordinates      x0:0~127
               y    coordinates      y0:0~7
               *asc asc String
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_ShowStr6x8(uint8_t shiny,uint8_t x,uint8_t y,uint8_t asc[]);




/** 
* @name: OLED_ShowStr8x16 
* This function deal with the oled Show asc String.
* @param[in]   shiny shiny
               x    coordinates      x0:0~127
               y    coordinates      y0:0~7
               *asc asc String
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_ShowStr8x16(uint8_t shiny,uint8_t x,uint8_t y,uint8_t asc[]);



/** 
* @name: OLED_ShowCN_16x16 
* This function deal with the oled Show Chinese characters.
* @param[in]   shiny shiny
               x    coordinates      x0:0~127
               y    coordinates      y0:0~7
               *CN_16x16   Chinese characters
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_ShowCN_16x16(uint8_t shiny,uint8_t x,uint8_t y,const uint8_t *CN_16x16);

/** 
* @name: OLED_DrawBMP 
* This function handles the output of the picture.
* @param[in]   x0   Starting point coordinates      x0:0~127
* @param[in]   y0   Starting point coordinates      y0:0~7
* @param[in]   x1   End the coordinates             x1:1~128
* @param[in]   y1   End the coordinates             y1:1~8
* @param[in]   *BMP picture
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,const uint8_t BMP[]);


#ifdef __cplusplus
}
#endif

#endif  /* COT_MT_OLED_H */

