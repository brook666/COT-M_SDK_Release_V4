/**
* @file         cot_mt_oled.c
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       wende.wu
* @date         2017-06-01
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
#include "iic.h"
#include "error.h"
#include "cot_mt_oled.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/

#define OLED_IIC_ADDR           0x3C   //   // oled iic address
#define OLED_WRITE_COMMAND      0x00
#define OLED_WRITE_DATA         0x40


const uint8_t ASC_F6x8[][6] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// sp
    0x00, 0x00, 0x00, 0x2f, 0x00, 0x00,// !
    0x00, 0x00, 0x07, 0x00, 0x07, 0x00,// "
    0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14,// #
    0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12,// $
    0x00, 0x62, 0x64, 0x08, 0x13, 0x23,// %
    0x00, 0x36, 0x49, 0x55, 0x22, 0x50,// &
    0x00, 0x00, 0x05, 0x03, 0x00, 0x00,// '
    0x00, 0x00, 0x1c, 0x22, 0x41, 0x00,// (
    0x00, 0x00, 0x41, 0x22, 0x1c, 0x00,// )
    0x00, 0x14, 0x08, 0x3E, 0x08, 0x14,// *
    0x00, 0x08, 0x08, 0x3E, 0x08, 0x08,// +
    0x00, 0x00, 0x00, 0xA0, 0x60, 0x00,// ,
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08,// -
    0x00, 0x00, 0x60, 0x60, 0x00, 0x00,// .
    0x00, 0x20, 0x10, 0x08, 0x04, 0x02,// /
    0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
    0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,// 1
    0x00, 0x42, 0x61, 0x51, 0x49, 0x46,// 2
    0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,// 3
    0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,// 4
    0x00, 0x27, 0x45, 0x45, 0x45, 0x39,// 5
    0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
    0x00, 0x01, 0x71, 0x09, 0x05, 0x03,// 7
    0x00, 0x36, 0x49, 0x49, 0x49, 0x36,// 8
    0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,// 9
    0x00, 0x00, 0x36, 0x36, 0x00, 0x00,// :
    0x00, 0x00, 0x56, 0x36, 0x00, 0x00,// ;
    0x00, 0x08, 0x14, 0x22, 0x41, 0x00,// <
    0x00, 0x14, 0x14, 0x14, 0x14, 0x14,// =
    0x00, 0x00, 0x41, 0x22, 0x14, 0x08,// >
    0x00, 0x02, 0x01, 0x51, 0x09, 0x06,// ?
    0x00, 0x32, 0x49, 0x59, 0x51, 0x3E,// @
    0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C,// A
    0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,// B
    0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,// C
    0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,// D
    0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,// E
    0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,// F
    0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,// G
    0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,// H
    0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,// I
    0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,// J
    0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,// K
    0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,// L
    0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,// M
    0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,// N
    0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,// O
    0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,// P
    0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
    0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,// R
    0x00, 0x46, 0x49, 0x49, 0x49, 0x31,// S
    0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,// T
    0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,// U
    0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,// V
    0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,// W
    0x00, 0x63, 0x14, 0x08, 0x14, 0x63,// X
    0x00, 0x07, 0x08, 0x70, 0x08, 0x07,// Y
    0x00, 0x61, 0x51, 0x49, 0x45, 0x43,// Z
    0x00, 0x00, 0x7F, 0x41, 0x41, 0x00,// [
    0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55,// 55
    0x00, 0x00, 0x41, 0x41, 0x7F, 0x00,// ]
    0x00, 0x04, 0x02, 0x01, 0x02, 0x04,// ^
    0x00, 0x40, 0x40, 0x40, 0x40, 0x40,// _
    0x00, 0x00, 0x01, 0x02, 0x04, 0x00,// '
    0x00, 0x20, 0x54, 0x54, 0x54, 0x78,// a
    0x00, 0x7F, 0x48, 0x44, 0x44, 0x38,// b
    0x00, 0x38, 0x44, 0x44, 0x44, 0x20,// c
    0x00, 0x38, 0x44, 0x44, 0x48, 0x7F,// d
    0x00, 0x38, 0x54, 0x54, 0x54, 0x18,// e
    0x00, 0x08, 0x7E, 0x09, 0x01, 0x02,// f
    0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C,// g
    0x00, 0x7F, 0x08, 0x04, 0x04, 0x78,// h
    0x00, 0x00, 0x44, 0x7D, 0x40, 0x00,// i
    0x00, 0x40, 0x80, 0x84, 0x7D, 0x00,// j
    0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,// k
    0x00, 0x00, 0x41, 0x7F, 0x40, 0x00,// l
    0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,// m
    0x00, 0x7C, 0x08, 0x04, 0x04, 0x78,// n
    0x00, 0x38, 0x44, 0x44, 0x44, 0x38,// o
    0x00, 0xFC, 0x24, 0x24, 0x24, 0x18,// p
    0x00, 0x18, 0x24, 0x24, 0x18, 0xFC,// q
    0x00, 0x7C, 0x08, 0x04, 0x04, 0x08,// r
    0x00, 0x48, 0x54, 0x54, 0x54, 0x20,// s
    0x00, 0x04, 0x3F, 0x44, 0x40, 0x20,// t
    0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C,// u
    0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,// v
    0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C,// w
    0x00, 0x44, 0x28, 0x10, 0x28, 0x44,// x
    0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,// y
    0x00, 0x44, 0x64, 0x54, 0x4C, 0x44,// z
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14,// horiz lines
    #if 0
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00,// RSSI  1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x40, 0x60, 0x60, 0x00, 0x00,// RSSI  2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x40, 0x60, 0x60, 0x70, 0x70,// RSSI  3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x40, 0x60, 0x60, 0x70, 0x70,// RSSI  4
    0x78, 0x78, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x40, 0x60, 0x60, 0x70, 0x70,// RSSI  5
    0x78, 0x78, 0x7C, 0x7C, 0x00, 0x00,//  
   #else
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00,// RSSI  1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x00, 0x60, 0x00, 0x00, 0x00,// RSSI  2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x00, 0x60, 0x00, 0x70, 0x00,// RSSI  3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x00, 0x60, 0x00, 0x70, 0x00,// RSSI  4
    0x78, 0x00, 0x00, 0x00, 0x00, 0x00,//
    
    0x40, 0x00, 0x60, 0x00, 0x70, 0x00,// RSSI  5
    0x78, 0x00, 0x7C, 0x00, 0x00, 0x00,//  

   #endif
};

/****************************************8*16的点阵************************************/
const uint8_t ASC_F8x16[]=	  
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//! 1
  0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//" 2
  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//# 3
  0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$ 4
  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//% 5
  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//& 6
  0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//' 7
  0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//( 8
  0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//) 9
  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//* 10
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+ 11
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//, 12
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//- 13
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//. 14
  0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,/// 15
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0 16
  0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,// 1 17
  0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,// 2 18
  0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,// 3 19
  0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,// 4 20
  0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5 21
  0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6 22
  0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7 23
  0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8 24
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9 25
  0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//: 26
  0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//; 27
  0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//< 28
  0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//= 29
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//> 30
  0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//? 31
  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@ 32
  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A 33
  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B 34
  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C 35
  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D 36
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E 37
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F 38
  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G 39
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H 40
  0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I 41
  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J 42
  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K 43
  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L 44
  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M 45
  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N 46
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O 47
  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P 48
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q 49
  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R 50
  0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S 51
  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T 52
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U 53
  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V 54
  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W 55
  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X 56
  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y 57
  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z 58
  0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[ 59
  0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\ 60
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//] 61
  0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^ 62
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_ 63
  0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//` 64
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a 65
  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b 66
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c 67
  0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d 68
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e 69
  0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f 70
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g 71
  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h 72
  0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i 73
  0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j 74
  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k 75
  0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l 76
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m 77
  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n 78
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o 79
  0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p 80
  0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q 81
  0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r 82
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s 83
  0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t 84
  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u 85
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v 86
  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w 87
  0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x 88
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y 89
  0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z 90
  0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{ 91
  0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//| 92
  0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//} 93
  0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~ 94
};

/***************************16*16的点阵字体取模方式：共阴——列行式——逆向输出*********/
const uint8_t CN_F16_16[] =
{

    0x00,0x00,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x00,0x00,
    0x00,0x04,0x84,0x44,0x24,0x17,0x04,0x04,0x04,0x04,0x17,0x24,0x44,0x84,0x04,0x00,//共 0
    
    0x00,0xFE,0x02,0x22,0xDA,0x06,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0xFE,0x00,0x00,
    0x00,0xFF,0x08,0x10,0x88,0x47,0x30,0x0F,0x02,0x02,0x02,0x42,0x82,0x7F,0x00,0x00,//阴1
    
    0x00,0x00,0xC0,0x40,0x40,0x40,0x7F,0x48,0x48,0x48,0x48,0xC8,0x08,0x08,0x00,0x00,
    0x80,0x40,0x37,0x04,0x04,0x14,0x64,0x04,0x14,0x64,0x04,0x07,0x10,0xE0,0x00,0x00,//点2
    
    0x00,0xFE,0x02,0x22,0xDA,0x06,0x08,0xC8,0xB8,0x8F,0xE8,0x88,0x88,0x88,0x08,0x00,
    0x00,0xFF,0x08,0x10,0x08,0x07,0x08,0x08,0x08,0x08,0xFF,0x08,0x08,0x08,0x08,0x00,//阵3 
};

const uint8_t PictureCode_COT[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xF8,0xFC,0xFC,0xFC,
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF8,0xF0,0x70,0x20,0x00,0x00,0x00,0x80,0x80,0xC0,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,
    0xF8,0xF8,0xF8,0xF8,0xF8,0xF0,0xF0,0xE0,0xE0,0xC0,0xC0,0x80,0x04,0x0C,0x1C,0x3C,0x7C,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x3F,0x1F,
    0x1F,0x1F,0x1F,0x1F,0x1F,0x3F,0x3F,0x3F,0x3F,0x0F,0x03,0x01,0xC0,0xF0,0xF8,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x7F,0x3F,0x3F,0x1F,0x1F,0x1F,0x1F,
    0x1F,0x3F,0x3F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF8,0xE0,0x80,0x01,0x03,0x07,0x0F,0x1F,0x1F,0x1F,0x1F,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x03,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xF0,0xE0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x80,0xC0,0xC0,0xF0,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
    0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFC,0xF8,0xE0,0xC0,0x83,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x3F,0x1F,0x0F,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x07,0x07,0x07,0x07,0x07,0x07,
    0x07,0x07,0x07,0x03,0x03,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/
/* cot application protocal parameters */


/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/



/***************************************************************************************************
 *                                          PUBLIC FUNCTIONS
 ***************************************************************************************************/


void I2C_WriteByte(int32_t iic_fd,uint8_t Command,uint8_t Data)//写命令
{
    uint8_t tmp_buf[2];
    tmp_buf[0] = Command;
    tmp_buf[1] = Data;
    if(iic_fd != 0)
        write(iic_fd,tmp_buf, 2);
}

void WriteCmd(int32_t iic_fd,uint8_t I2C_Command)//写命令
{
    I2C_WriteByte(iic_fd,OLED_WRITE_COMMAND, I2C_Command);
}

void WriteDat(int32_t iic_fd,uint8_t I2C_Data)//写数据
{
	I2C_WriteByte(iic_fd,OLED_WRITE_DATA, I2C_Data);
}

void OLED_SetPos(int32_t iic_fd,uint8_t x,uint8_t y) //设置起始点坐标
{ 
	WriteCmd(iic_fd,0xb0+y);
	WriteCmd(iic_fd,((x&0xf0)>>4)|0x10);
	WriteCmd(iic_fd,(x&0x0f)|0x01);
}


/** 
* @name: OLED_register_init 
* This function deal with the oled register init.
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
void OLED_register_init( )
{
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )   return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
	am_util_delay_ms(100); // Waiting  
	
	WriteCmd(iic_fd,0xAE); //display off
	WriteCmd(iic_fd,0x20); //Set Memory Addressing Mode	
	WriteCmd(iic_fd,0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(iic_fd,0xb0); //Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(iic_fd,0xc8); //Set COM Output Scan Direction
	WriteCmd(iic_fd,0x00); //---set low column address
	WriteCmd(iic_fd,0x10); //---set high column address
	WriteCmd(iic_fd,0x40); //--set start line address
	WriteCmd(iic_fd,0x81); //--set contrast control register
	WriteCmd(iic_fd,0xff); //亮度调节 0x00~0xff
	WriteCmd(iic_fd,0xa1); //--set segment re-map 0 to 127
	WriteCmd(iic_fd,0xa6); //--set normal display
	WriteCmd(iic_fd,0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(iic_fd,0x3F); //
	WriteCmd(iic_fd,0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(iic_fd,0xd3); //-set display offset
	WriteCmd(iic_fd,0x00); //-not offset
	WriteCmd(iic_fd,0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(iic_fd,0xf0); //--set divide ratio
	WriteCmd(iic_fd,0xd9); //--set pre-charge period
	WriteCmd(iic_fd,0x22); //
	WriteCmd(iic_fd,0xda); //--set com pins hardware configuration
	WriteCmd(iic_fd,0x12);
	WriteCmd(iic_fd,0xdb); //--set vcomh
	WriteCmd(iic_fd,0x20); //0x20,0.77xVcc
	WriteCmd(iic_fd,0x8d); //--set DC-DC enable
	WriteCmd(iic_fd,0x14); //
	WriteCmd(iic_fd,0xaf); //--turn on oled panel
    close(iic_fd);	
}




uint8_t OLED_ShowShiny(uint8_t Dat)
{
    return ~Dat;
}



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
void OLED_Fill(uint8_t lattice)//全屏填充
{
	uint8_t m,n;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )   return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
    
	for(m=0;m<8;m++)
	{
		WriteCmd(iic_fd,0xb0+m);		//page0-page1
		WriteCmd(iic_fd,0x00);		//low column start address
		WriteCmd(iic_fd,0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			WriteDat(iic_fd,lattice);
		}
	}
    close(iic_fd);
}

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
void OLED_CLS()//清屏
{
	OLED_Fill(0x00);
}

void oled_line_cls( uint8_t m )
{
	uint8_t n;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )   return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
    
	WriteCmd(iic_fd,0xb0+m);		//page0-page1
	WriteCmd(iic_fd,0x00);		//low column start address
	WriteCmd(iic_fd,0x10);		//high column start address
	for(n=0;n<128;n++)
	{
		WriteDat(iic_fd,0x00);
	}

    close(iic_fd);
}

/** 
* @name: OLED_ShowStr6x8 
* This function deal with the oled Show asc String.
* @param[in]   x    coordinates      x0:0~127
* @param[in]   y    coordinates      y0:0~7
* @param[in]   *asc asc String
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
void OLED_ShowStr6x8(uint8_t shiny,uint8_t x,uint8_t y,uint8_t asc[])
{
	unsigned char c = 0,i = 0,j = 0;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )   return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
	while(asc[j] != '\0')
	{
		c = asc[j] - 0x20;
		if(x >= 126) //if(x > 126)
		{
			x = 0;
			y++;
		}
		OLED_SetPos(iic_fd,x,y);
        if(shiny == OLED_SHOWLIGHT)
        {
            for(i=0;i<6;i++)
		        WriteDat(iic_fd,OLED_ShowShiny(ASC_F6x8[c][i]));
        }    
        else
        {
            for(i=0;i<6;i++)
		        WriteDat(iic_fd,ASC_F6x8[c][i]);
        }
		x += 6;
		j++;
	}
    close(iic_fd);
}


/** 
* @name: OLED_ShowStr8x16 
* This function deal with the oled Show asc String.
* @param[in]   x    coordinates      x0:0~127
* @param[in]   y    coordinates      y0:0~7
* @param[in]   *asc asc String
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
void OLED_ShowStr8x16(uint8_t shiny,uint8_t x,uint8_t y,uint8_t asc[])
{
	uint8_t c = 0,i = 0,j = 0;
    uint8_t len = 8;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )      return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
	while(asc[j] != '\0')
	{
		c = asc[j] - 0x20;
		if(x >= 128)
		{
			x = 0;
			y+=2;//y++;
		}
		OLED_SetPos(iic_fd,x,y);
        if(x == 120) len = 7;
        else    len = 8;
        for(i=0;i < len;i++)
        {
            if(shiny == OLED_SHOWLIGHT)
		        WriteDat(iic_fd,OLED_ShowShiny(ASC_F8x16[c*16+i]));
            else
                WriteDat(iic_fd,ASC_F8x16[c*16+i]);
        } 
		OLED_SetPos(iic_fd,x,y+1);
        for(i=0;i < len;i++)
        {
            if(shiny == OLED_SHOWLIGHT)
			    WriteDat(iic_fd,OLED_ShowShiny(ASC_F8x16[c*16+i+8]));
            else
                WriteDat(iic_fd,ASC_F8x16[c*16+i+8]);
        } 
		x += 8;
		j++;
	}
    close(iic_fd);
}


/** 
* @name: OLED_ShowCN_16x16 
* This function deal with the oled Show Chinese characters.
* @param[in]   x    coordinates      x0:0~127
* @param[in]   y    coordinates      y0:0~7
* @param[in]   *CN_16x16   Chinese characters
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
void OLED_ShowCN_16x16(uint8_t shiny,uint8_t x,uint8_t y,const uint8_t *CN_16x16)
{
	uint8_t wm = 0,len = 16;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if(iic_fd <= 0 )  return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);
	OLED_SetPos(iic_fd,x , y);
    if(x == 112) len = 15;
        else    len = 16;
    for(wm = 0;wm < len;wm++)
	{
	    if(shiny == OLED_SHOWLIGHT)
		    WriteDat(iic_fd,OLED_ShowShiny(CN_16x16[wm]));
        else
            WriteDat(iic_fd,CN_16x16[wm]);
	}
	OLED_SetPos(iic_fd,x,y + 1);
    for(wm = 0;wm < len;wm++)
	{
	    if(shiny == OLED_SHOWLIGHT)
		    WriteDat(iic_fd,OLED_ShowShiny(CN_16x16[wm + 16]));
        else
            WriteDat(iic_fd,CN_16x16[wm + 16]);
	}
    close(iic_fd);
}


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
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,const uint8_t BMP[])
{
	uint16_t j = 0;
	uint8_t x,y;
    int32_t iic_fd = 0;  
        
    iic_fd = open(PATH_IIC0,0);
    if( iic_fd <= 0 )   return ;
    ioctl(iic_fd,IIC_SLAVE_ADDR,OLED_IIC_ADDR);

    if(y1%8==0)  y = y1/8;
    else         y = y1/8 + 1;
    for(y=y0;y<y1;y++)
    {
        OLED_SetPos(iic_fd,x0,y);
        for(x=x0;x<x1;x++)
        {
        	WriteDat(iic_fd,BMP[j++]);
        }
    }
    close(iic_fd);
}


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
int32_t cot_mt_oled_init( void )
{
    iic_init_module();
    OLED_register_init();
    

    OLED_DrawBMP(0,0,128,8,PictureCode_COT); 
    am_util_delay_ms(1000);
    OLED_CLS();//清屏  

    #if 0
    OLED_CLS();//清屏        
    am_util_delay_ms(1000);
    OLED_Fill(0xFF);//全屏点亮
    am_util_delay_ms(1000);
    OLED_Fill(0x00);//全屏灭
        OLED_ShowStr6x8(OLED_SHOWLIGHT,0,0,"1234567890abcdef1234567890abcdef");        
        OLED_ShowStr8x16(OLED_SHOWLIGHT,0,2,"1234567890ABCDEF1234567890ABCDEF");
        OLED_ShowCN_16x16(OLED_SHOWLIGHT,0,4,CN_F16_16);
        OLED_ShowCN_16x16(OLED_SHOWLIGHT,16,4,CN_F16_16 + 32);
        OLED_ShowCN_16x16(OLED_SHOWLIGHT,32,4,CN_F16_16 + 32 * 2);
        OLED_ShowCN_16x16(OLED_SHOWLIGHT,112,4,CN_F16_16 + 32 * 3);
        
        am_util_delay_ms(10000);
        am_util_delay_ms(10000);
        
        OLED_ShowStr6x8(OLED_SHOWDARK,0,0,"1234567890abcdef1234567890abcdef");
        OLED_ShowStr8x16(OLED_SHOWDARK,0,2,"1234567890ABCDEF1234567890ABCDEF");
        OLED_ShowCN_16x16(OLED_SHOWDARK,0,4,CN_F16_16);
        OLED_ShowCN_16x16(OLED_SHOWDARK,16,4,CN_F16_16 + 32);
        OLED_ShowCN_16x16(OLED_SHOWDARK,32,4,CN_F16_16 + 32 * 2);
        OLED_ShowCN_16x16(OLED_SHOWDARK,48,4,CN_F16_16 + 32 * 3);

        //OLED_DrawBMP(0,0,128,8,PictureCode); 
        
        
        am_util_delay_ms(6000);
        OLED_Fill(0x00);//全屏灭
    #endif
    
    return 0;
}




