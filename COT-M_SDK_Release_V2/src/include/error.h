/*!
 * \file error.h
 * \brief define return code
 */
#ifndef __ERROR_H__
#define __ERROR_H__


/* generic error */
#define ERR_SUCCESS	 			(0)         /*!< ok */
#define ERR_ERROR				(-1)
#define ERR_UNDEFINE			(-2)        /*!< unknown error */
#define ERR_PARAMETER			(-3)        /*!< invalid parameter */
#define ERR_TIME_OUT			(-4)        /*!< time is out */
#define ERR_PARITY              (-5)        /* */
#define ERR_FAILUER             (-6)        /* */


/* buffer error */
#define ERR_BUFFER_INITIALIZED  (-10)       /*!< buffer is not initalized */
#define ERR_BUFFER_EMPTY        (-11)       /*!< buffer is empty */
#define ERR_BUFFER_FULL         (-12)       /*!< buffer is full */

/* flash error */
#define ERR_FLASH_PROGRAM       (-20)       /*!< error on flash programming */
#define ERR_FLASH_ERASE         (-21)       /*!< error on flash erasing */





#endif

