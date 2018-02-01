/**
* @file         cot_mt.h
* @brief        MonitorTest command and response definitions.
* @details      This is the detail description.
* @author       zhangjh
* @date         2017-06-01
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/


/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

//#include "ZComDef.h"
#include <stdint.h>
#include "cot_mt.h"
#include "OSAL.h"
#include "OSAL_Memory.h"

#include "uart.h"
//#include "hal_led.h"
//#include "hal_key.h"
#include "cot_mt_uart.h"

/**************************************************************************************************
 * Type define
 **************************************************************************************************/
typedef uint8_t ( *mtProcessMsg_t)(uint8_t *p_buf);

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

mtProcessMsg_t mtProcessIncoming[] =
{
  NULL,                               // MT_RPC_SYS_RES0

#if defined (MT_SYS_FUNC)
  MT_SysCommandProcessing,            // MT_RPC_SYS_SYS
#else
  NULL,
#endif

#if defined (MT_MAC_FUNC)
  MT_MacCommandProcessing,            // MT_RPC_SYS_MAC
#else
  NULL,
#endif

#if defined (MT_NWK_FUNC)
  MT_NwkCommandProcessing,            // MT_RPC_SYS_NWK
#else
  NULL,
#endif

#if defined (MT_AF_FUNC)
  MT_AfCommandProcessing,             // MT_RPC_SYS_AF
#else
  NULL,
#endif

#if defined (MT_ZDO_FUNC)
  MT_ZdoCommandProcessing,            // MT_RPC_SYS_ZDO
#else
  NULL,
#endif

#if defined (MT_SAPI_FUNC)
  MT_SapiCommandProcessing,           // MT_RPC_SYS_SAPI
#else
  NULL,
#endif

#if defined (MT_UTIL_FUNC)
  MT_UtilCommandProcessing,           // MT_RPC_SYS_UTIL
#else
  NULL,
#endif

#if defined (MT_DEBUG_FUNC)
  MT_DebugCommandProcessing,          // MT_RPC_SYS_DBG
#else
  NULL,
#endif

#if defined (MT_APP_FUNC)
  MT_AppCommandProcessing,            // MT_RPC_SYS_APP
#else
  NULL,
#endif

#if defined (MT_OTA_FUNC)
  MT_OtaCommandProcessing,            // MT_RPC_SYS_OTA
#else
  NULL,
#endif

#if defined (MT_ZNP_FUNC)
  MT_ZnpCommandProcessing,
#else
  NULL,
#endif

  NULL,  // Spare sub-system 12.

#if defined (MT_UBL_FUNC)
  MT_UblCommandProcessing,
#else
  NULL,
#endif

  NULL,                               // MT_RPC_SYS_RESERVED14

#if defined (MT_APP_CNF_FUNC)
  MT_AppCnfCommandProcessing,        // MT_RPC_SYS_APP_CNF
#else
  NULL,
#endif

  NULL,                               // MT_RPC_SYS_RESERVED16
#if defined (MT_MAC_PROTOBUF_FUNC)
  MT_MacPBCmdProc,                    // MT_RPC_SYS_PROTOBUF
#else
  NULL,
#endif
  NULL,                               // MT_RPC_SYS_RES18
  NULL,                               // MT_RPC_SYS_RES19
  NULL,                               // MT_RPC_SYS_RES20
#if defined (MT_GP_CB_FUNC)
  MT_GpCommandProcessing,
#else
  NULL,
#endif
};

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
uint8_t MT_TaskID;
/**************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

void MT_MsgQueueInit( void );
void MT_ResetMsgQueue( void );
uint8_t MT_QueueMsg( uint8_t *msg , uint8_t len );
void MT_ProcessQueue( void );

#if defined ( MT_USER_TEST_FUNC )
void MT_ProcessAppUserCmd( uint8_t *pData );
#endif

/**************************************************************************************************
 * @fn         MT_Init
 *
 * @brief      This function is the secondary initialization that resolves conflicts during
 *             osalInitTasks(). For example, since MT is the highest priority task, and
 *             specifically because the MT task is initialized before the ZDApp task, if MT_Init()
 *             registers anything with ZDO_RegisterForZdoCB(), it is wiped out when ZDApp task
 *             initialization invokes ZDApp_InitZdoCBFunc().
 *             There may be other existing or future such races, so try to do all possible
 *             MT initialization here vice in MT_TaskInit().
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void MT_Init(uint8_t taskID)
{
  MT_TaskID = taskID;

}

/***************************************************************************************************
 * @fn      MT_ProcessIncoming
 *
 * @brief  Process Incoming Message.
 *
 * @param   byte *pBuf - pointer to event message
 *
 * @return  void
 ***************************************************************************************************/
void MT_ProcessIncoming(uint8_t *pBuf)
{
#if 0
  mtProcessMsg_t func;
  uint8_t rsp[MT_RPC_FRAME_HDR_SZ];

  /* pre-build response message:  | status | cmd0 | cmd1 | */
  rsp[1] = pBuf[MT_RPC_POS_CMD0];
  rsp[2] = pBuf[MT_RPC_POS_CMD1];

  /* check length */
  if (pBuf[MT_RPC_POS_LEN] > MT_RPC_DATA_MAX)
  {
    rsp[0] = MT_RPC_ERR_LENGTH;
  }
  /* check subsystem range */
  else if ((rsp[1] & MT_RPC_SUBSYSTEM_MASK) < MT_RPC_SYS_MAX)
  {
    /* look up processing function */
    func = mtProcessIncoming[rsp[1] & MT_RPC_SUBSYSTEM_MASK];
    if (func)
    {
      /* execute processing function */
      rsp[0] = (*func)(pBuf);
    }
    else
    {
      rsp[0] = MT_RPC_ERR_SUBSYSTEM;
    }
  }
  else
  {
    rsp[0] = MT_RPC_ERR_SUBSYSTEM;
  }

  /* if error and this was an SREQ, send error message */
  if ((rsp[0] != MT_RPC_SUCCESS) && ((rsp[1] & MT_RPC_CMD_TYPE_MASK) == MT_RPC_CMD_SREQ))
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_RES0), 0,
                                                                  MT_RPC_FRAME_HDR_SZ, rsp);
  }
#endif

}

/***************************************************************************************************
 * @fn      MTProcessAppRspMsg
 *
 * @brief   Process the User App Response Message
 *
 * @param   data - output serial buffer.  The first byte must be the
 *          endpoint that send this message.
 * @param   len - data length
 *
 * @return  none
 ***************************************************************************************************/
void MTProcessAppRspMsg( uint8_t *pData, uint8_t len )
{
  /* Send out Reset Response message */
  //MT_BuildAndSendZToolResponse( ((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), MT_APP_RSP, len, pData );
}


/***************************************************************************************************
 * @fn      MT_ReverseBytes
 *
 * @brief
 *
 *   Reverses bytes within an array
 *
 * @param   data - ptr to data buffer to reverse
 * @param    len - number of bytes in buffer
 *
 * @return  void
 ***************************************************************************************************/
void MT_ReverseBytes( uint8_t *pData, uint8_t len )
{
  uint8_t i,j;
  uint8_t temp;

  for ( i = 0, j = len-1; len > 1; len-=2 )
  {
    temp = pData[i];
    pData[i++] = pData[j];
    pData[j--] = temp;
  }
}


/***************************************************************************************************
 * @fn      MT_Word2Buf
 *
 * @brief   Copy a uint16 array to a byte array, little endian.
 *
 * @param   pBuf - byte array
 * @param   pWord - uint16 array
 * @param   len - length of uint16 array
 *
 * @return  pointer to end of byte array
 ***************************************************************************************************/
uint8_t *MT_Word2Buf( uint8_t *pBuf, uint16_t *pWord, uint8_t len )
{
  while ( len-- > 0 )
  {
    //*pBuf++ = LO_UINT16( *pWord );
    //*pBuf++ = HI_UINT16( *pWord );
    pWord++;
  }

  return pBuf;
}

