/**
* @file         cot_physical_layer.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       zhangjh
* @date     	2017-0429
* @version  	V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_MAC_H
#define COT_MAC_H


/*********************************************************************
 * INCLUDE
 */
#include <stdint.h>
#include "osal.h"
#include "rf_param_type_define.h"


/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 */

/*!
 * cot phy layer cmd
 */
#define MAC_EVENT_INIT_CMD          0x01
#define MAC_EVENT_CONFIG_CMD        0x02
#define MAC_EVENT_TX_CMD            0x03
#define MAC_EVENT_RX_CMD            0x04
#define MAC_EVENT_FREE_CMD          0x05
#define MAC_EVENT_TX_TIMEOUT_CMD    0x06
#define MAC_EVENT_TX_ERROR_CMD      0x07
#define MAC_EVENT_RX_TIMEOUT_CMD    0x08
#define MAC_EVENT_RX_ERROR_CMD      0x09
#define MAC_EVENT_RNG_DONE_CMD      0x0a
#define MAC_EVENT_TX_DONE_CMD       0x0b
#define MAC_EVENT_RX_DONE_CMD       0x0c
#define MAC_EVENT_LAYER_STATUS      0x0d
#define MAC_EVENT_MSG_SLEEP         0x0e
#define MAC_EVENT_RTC_AWAK			0x0f


/*!
 * \brief Define sending package type
 */
typedef enum
{
	UNCONFIRM            = 0x0c,    //unconfirm packages in sending
    CONFIRM              = 0x0d     //confirm packages in sending
}pkg_type_t;



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*!
 * \brief Define current demo mode
 */
typedef enum
{
    PKG_BEACON              = 0x00,
    PKG_ACK                 = 0x01,
    PKG_COMMAND             = 0x02,
    PKG_NORMAL_STREAM_UP    = 0x0c,
    PKG_EMERG_STREAM_UP     = 0x0d,
    PKG_NORMAL_STREAM_DOWN  = 0x0e,
    PKG_EMERG_STREAM_DOWN   = 0x0f,
}pkg_header_type_t;


/*!
 * cot mac header field definition (PHDR field)
 *
 * COTWAN Specification VX.X.X, chapter X.X
 */
typedef union uCOTMACIRQType
{
    /*!
     * Byte-access to the bits
     */
    uint16_t Value;
    /*!
     * Structure containing single access to irq type bits
     */
    struct sIrqTypeBits
    {
        /*!
         * TxDone
         */
        uint16_t tx_done           		: 1;
        /*!
         * RxDone
         */
        uint16_t rx_done           		: 1;
        /*!
         * SyncWordValid
         */
        uint16_t sync_word_valid         : 1;

    }Bits;
}cot_phy_irq_type_t;


/*!
 * cot mac header field definition (PHDR field)
 *
 * COTWAN Specification VX.X.X, chapter X.X
 */
typedef union uCOTPKGHeader
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * Structure containing single access to header bits
     */
    struct sHdrBits
    {
        /*!
         * reserved
         */
        uint8_t reserved            : 2;

        /*!
         * FramePending
         */
        uint8_t frame_pending       : 1;

        /*!
         * FramePending
         */
        uint8_t ack_request         : 1;


        /*!
         * Message type
         */
        uint8_t MType               : 4;

    }Bits;
}cot_pkg_header_t;


/*!
 * \brief Represents an amount of time measurable by the radio clock
 *
 * @code
 * Time = Step * NbSteps
 * Example:
 * Step = RADIO_TICK_SIZE_4000_US( 4 ms )
 * NbSteps = 1000
 * Time = 4e-3 * 1000 = 4 seconds
 * @endcode
 */
typedef struct
{
    RF_RadioTickSizes_t Step;                                  //!< The step of ticktime
    /*!
     * \brief The number of steps for ticktime
     * Special values are:
     *     - 0x0000 for single mode
     *     - 0xFFFF for continuous mode
     */
    uint16_t NbSteps;
}cot_phy_TickTime_t;


typedef struct
{
    osal_event_hdr_t            hdr;                    //osal event head
    uint8_t                     event_cmd;              //event cmd
    uint8_t  					src_task_id;			//the osal task id of the message sendder
	RF_RadioPacketTypes_t       ModulationType;      	//modulation type
	int8_t                      rssi;                   //only effect when recevie
	cot_phy_irq_type_t          irq_type;               //only effect when rx mac event
	cot_phy_TickTime_t          tick_timeout;           //cot phy tick timetout,only effect when rx mac event
	cot_pkg_header_t            pkg_hdr;                //
	int16_t						rssi_water_leve;		//
	uint32_t                    src_addr;               //only effect in mac rx event
	uint32_t                    dst_addr;               //destination addr
	uint32_t                    data_len;			    //application data length
	uint8_t                     *data;                  //application data pointer
}cot_mac_msg_t;


/*!
 * \brief The type describing the modulation parameters for every packet types
 */
typedef struct
{
	//RF_RadioPacketTypes_t                    PacketType;        //!< Packet to which the modulation parameters are referring to.

    struct
    {
        /*!
         * \brief Holds the GFSK modulation parameters
         *
         * In GFSK modulation, the bit-rate and bandwidth are linked together. In this structure, its values are set using the same token.
         */
        struct
        {
            RF_RadioGfskBleBitrates_t    BitrateBandwidth;  //!< The bandwidth and bit-rate values for BLE and GFSK modulations
            RF_RadioGfskBleModIndexes_t  ModulationIndex;   //!< The coding rate for BLE and GFSK modulations
            RF_RadioModShapings_t        ModulationShaping; //!< The modulation shaping for BLE and GFSK modulations
        }Gfsk;
        /*!
         * \brief Holds the LORA modulation parameters
         *
         * LORA modulation is defined by Spreading Factor (SF), Bandwidth and Coding Rate
         */
        struct
        {
            RF_RadioLoRaSpreadingFactors_t  SpreadingFactor;   //!< Spreading Factor for the LORA modulation
            RF_RadioLoRaBandwidths_t        Bandwidth;         //!< Bandwidth for the LORA modulation
            RF_RadioLoRaCodingRates_t       CodingRate;        //!< Coding rate for the LORA modulation
        }LoRa;
        /*!
         * \brief Holds the FLRC modulation parameters
         *
         * In FLRC modulation, the bit-rate and bandwidth are linked together. In this structure, its values are set using the same token.
         */
        struct
        {
            RF_RadioFlrcBitrates_t          BitrateBandwidth;  //!< The bandwidth and bit-rate values for FLRC modulation
            RF_RadioFlrcCodingRates_t       CodingRate;        //!< The coding rate for FLRC modulation
            RF_RadioModShapings_t           ModulationShaping; //!< The modulation shaping for FLRC modulation
        }Flrc;
        /*!
         * \brief Holds the BLE modulation parameters
         *
         * In BLE modulation, the bit-rate and bandwidth are linked together. In this structure, its values are set using the same token.
         */
        struct
        {
            RF_RadioGfskBleBitrates_t       BitrateBandwidth;  //!< The bandwidth and bit-rate values for BLE and GFSK modulations
            RF_RadioGfskBleModIndexes_t     ModulationIndex;   //!< The coding rate for BLE and GFSK modulations
            RF_RadioModShapings_t           ModulationShaping; //!< The modulation shaping for BLE and GFSK modulations
        }Ble;
        struct
        {
            uint32_t    RngAddress;         // Ranging Address
            uint16_t    RngFullScale;       // Full range of measuring distance (Ranging)
            uint8_t     RngRequestCount;    // Ranging Request Count
            uint8_t     RngZscoreMax;       // Max Zscore for ranging value ( ! *10 ! )
            uint8_t     RngAntenna;         // Ranging antenna selection
            uint8_t     RngUnit;            // Ranging distance unit [m]/[mi]
            uint16_t    RngCalib;           // Ranging Calibration
            double      RngFeiFactor;       // Ranging frequency correction factor
            uint16_t    RngReqDelay;        // Time between ranging request
            RF_RadioLoRaSpreadingFactors_t  SpreadingFactor;   //!< Spreading Factor for the LORA modulation
            RF_RadioLoRaBandwidths_t        Bandwidth;         //!< Bandwidth for the LORA modulation
            RF_RadioLoRaCodingRates_t       CodingRate;        //!< Coding rate for the LORA modulation
        }Rng;

    }Params;                                                   //!< Holds the modulation parameters structure
}PHYModulationParams_t;

/*!
 * \brief The type describing the packet parameters for every packet types
 */
typedef struct
{
	//RF_RadioPacketTypes_t                    PacketType;        //!< Packet to which the modulation parameters are referring to.
    struct
    {
        /*!
         * \brief Holds the GFSK packet parameters
         */
        struct
        {
            RF_RadioPreambleLengths_t       PreambleLength;    //!< The preamble length for GFSK packet type
            RF_RadioSyncWordLengths_t       SyncWordLength;    //!< The synchronization word length for GFSK packet type
            RF_RadioSyncWordRxMatchs_t      SyncWordMatch;     //!< The synchronization correlator to use to check synchronization word
            RF_RadioPacketLengthModes_t     HeaderType;        //!< If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted
            uint8_t                         PayloadLength;     //!< Size of the payload in the GFSK packet
            RF_RadioCrcTypes_t              CrcLength;         //!< Size of the CRC block in the GFSK packet
            RF_RadioWhiteningModes_t        Whitening;         //!< Usage of whitening on payload and CRC blocks plus header block if header type is variable
        }Gfsk;
        /*!
         * \brief Holds the LORA packet parameters
         */
        struct
        {
            uint8_t                          PreambleLength;    //!< The preamble length is the number of LORA symbols in the preamble. To set it, use the following formula @code Number of symbols = PreambleLength[3:0] * ( 2^PreambleLength[7:4] ) @endcode
            RF_RadioLoRaPacketLengthsModes_t HeaderType;        //!< If the header is explicit, it will be transmitted in the LORA packet. If the header is implicit, it will not be transmitted
            uint8_t                          PayloadLength;     //!< Size of the payload in the LORA packet
            RF_RadioLoRaCrcModes_t           Crc;           	//!< Size of CRC block in LORA packet
            RF_RadioLoRaIQModes_t            InvertIQ;          //!< Allows to swap IQ for LORA packet
        }LoRa;
        /*!
         * \brief Holds the FLRC packet parameters
         */
        struct
        {
            RF_RadioPreambleLengths_t       PreambleLength;    //!< The preamble length for FLRC packet type
            RF_RadioFlrcSyncWordLengths_t   SyncWordLength;    //!< The synchronization word length for FLRC packet type
            RF_RadioSyncWordRxMatchs_t      SyncWordMatch;     //!< The synchronization correlator to use to check synchronization word
            RF_RadioPacketLengthModes_t     HeaderType;        //!< If the header is explicit, it will be transmitted in the FLRC packet. If the header is implicit, it will not be transmitted.
            uint8_t                         PayloadLength;     //!< Size of the payload in the FLRC packet
            RF_RadioCrcTypes_t              CrcLength;         //!< Size of the CRC block in the FLRC packet
            RF_RadioWhiteningModes_t        Whitening;         //!< Usage of whitening on payload and CRC blocks plus header block if header type is variable
        }Flrc;
        /*!
         * \brief Holds the BLE packet parameters
         */
        struct
        {
            RF_RadioBleConnectionStates_t    ConnectionState;   //!< The BLE state
            RF_RadioBleCrcTypes_t            CrcLength;         //!< Size of the CRC block in the BLE packet
            RF_RadioBleTestPayloads_t        BleTestPayload;    //!< Special BLE packet types
            RF_RadioWhiteningModes_t         Whitening;         //!< Usage of whitening on PDU and CRC blocks of BLE packet
        }Ble;
                /*!
         * \brief Holds the Rng packet parameters
         */
        struct
        {
            uint8_t                          PreambleLength;    //!< The preamble length is the number of LORA symbols in the preamble. To set it, use the following formula @code Number of symbols = PreambleLength[3:0] * ( 2^PreambleLength[7:4] ) @endcode
            RF_RadioLoRaPacketLengthsModes_t HeaderType;        //!< If the header is explicit, it will be transmitted in the LORA packet. If the header is implicit, it will not be transmitted
            uint8_t                          PayloadLength;     //!< Size of the payload in the LORA packet
            RF_RadioLoRaCrcModes_t           Crc;           	//!< Size of CRC block in LORA packet
            RF_RadioLoRaIQModes_t            InvertIQ;          //!< Allows to swap IQ for LORA packet
        }Rng;
    }Params;                                                 //!< Holds the packet parameters structure
}PHYPacketParams_t;


/*!
 * \brief Settings structure of rf_param structure
 */
#pragma pack(1)
typedef struct
{
    uint8_t     Entity;             // Master or Slave
    uint8_t     HoldDemo;           // Put demo in hold status
    uint8_t     AntennaSwitch;      // Witch antenna connected
    int8_t      TxPower;            // Demo Tx power
    uint32_t    Frequency;          // Demo frequency

    uint32_t    cot_dev_id;          //device id
    uint32_t    cot_multicast_id;    //multicast id
    uint32_t    cot_broadcast_id;    //broadcast id

    uint64_t    ChannelsCount;      // communication channel sequence

    RF_RadioRegulatorModes_t    RF_RadioRegulatorModes; // Radio Power Mode [0: LDO, 1:DC_DC]
    RF_RadioPacketTypes_t       RF_RadioPacketTypes;    // Demo modulation type (LORA, GFSK, FLRC)

    PHYModulationParams_t       PHYModulationParams;
    PHYPacketParams_t           PHYPacketParams;
}PHYParamSettings_t;
#pragma pack()


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

 /**
 * @name: phy_layer_init
 * This funtion deal with the phy layer init
 * @param[in]   task_id -- phy task id in osal.
 * @param[out]  none
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by zhangjh on 2017-04-29
 */
 void phy_layer_init( uint8_t taskId );


 uint8_t get_mac_layer_task_id(void);

/**
 * @name: phy_layer_processevent
 * This funtion deal with the phy tasks
 * @param[in]   task_id -- phy task id in osal.
 * @param[out]  events -- the event of the task_id.
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by zhangjh on 2017-04-29
 */
uint16_t cot_mac_layer_processevent(uint8_t task_id, uint16_t events);


/**
* @name: set_workparameters
* This funtion deal with the phy layer param init
* @param[in]   none.
* @param[out]  none.
* @retval  none.
* @retval
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-05-09
*/
void set_workparameters(void);

uint8_t ParametersSettings(PHYParamSettings_t *ParamSetting);



#endif
