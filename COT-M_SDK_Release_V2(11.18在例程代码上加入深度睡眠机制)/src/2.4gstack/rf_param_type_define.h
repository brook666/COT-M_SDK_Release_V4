/**
* @file         rf_param_type_define.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef RF_PARAM_TYPE_DEFINE_H
#define RF_PARAM_TYPE_DEFINE_H



/*!
 * \brief Define 3 preset central frequencies [Hz]
 */
#define RF_DEMO_CENTRAL_FREQ_PRESET1   2402000000UL
#define RF_DEMO_CENTRAL_FREQ_PRESET2   2450000000UL
#define RF_DEMO_CENTRAL_FREQ_PRESET3   2480000000UL



/*!
 * \brief Define current demo mode
 */
enum DemoMode
{
    RF_MASTER = 0,
    RF_SLAVE
};



/*!
 * \brief Declares the power regulation used to power the device
 *
 * This command allows the user to specify if DC-DC or LDO is used for power regulation.
 * Using only LDO implies that the Rx or Tx current is doubled
 */
typedef enum
{
    RF_USE_LDO                               = 0x00,           //! Use LDO (default value)
    RF_USE_DCDC                              = 0x01,           //! Use DCDC
}RF_RadioRegulatorModes_t;


/*!
 * \brief Represents the possible packet type (i.e. modem) used
 */
typedef enum
{
    RF_PACKET_TYPE_GFSK                        = 0x00,
    RF_PACKET_TYPE_LORA,
    RF_PACKET_TYPE_RANGING,
    RF_PACKET_TYPE_FLRC,
    RF_PACKET_TYPE_BLE,
    RF_PACKET_TYPE_NONE                        = 0x0F,
}RF_RadioPacketTypes_t;

/*!
 * \brief Represents the possible spreading factor values in LORA packet types
 */
typedef enum
{
    RF_LORA_SF5                                = 0x50,
    RF_LORA_SF6                                = 0x60,
    RF_LORA_SF7                                = 0x70,
    RF_LORA_SF8                                = 0x80,
    RF_LORA_SF9                                = 0x90,
    RF_LORA_SF10                               = 0xA0,
    RF_LORA_SF11                               = 0xB0,
    RF_LORA_SF12                               = 0xC0,
}RF_RadioLoRaSpreadingFactors_t;

/*!
 * \brief Represents the bandwidth values for LORA packet type
 */
typedef enum
{
    RF_LORA_BW_0200                            = 0x34,
    RF_LORA_BW_0400                            = 0x26,
    RF_LORA_BW_0800                            = 0x18,
    RF_LORA_BW_1600                            = 0x0A,
}RF_RadioLoRaBandwidths_t;

/*!
 * \brief Represents the coding rate values for LORA packet type
 */
typedef enum
{
    RF_LORA_CR_4_5                             = 0x01,
    RF_LORA_CR_4_6                             = 0x02,
    RF_LORA_CR_4_7                             = 0x03,
    RF_LORA_CR_4_8                             = 0x04,
    RF_LORA_CR_LI_4_5                          = 0x05,
    RF_LORA_CR_LI_4_6                          = 0x06,
    RF_LORA_CR_LI_4_7                          = 0x07,
}RF_RadioLoRaCodingRates_t;

/*!
 * \brief Holds the packet length mode of a LORA packet type
 */
typedef enum
{
    RF_LORA_PACKET_VARIABLE_LENGTH             = 0x00,         //!< The packet is on variable size, header included
    RF_LORA_PACKET_FIXED_LENGTH                = 0x80,         //!< The packet is known on both sides, no header included in the packet
    RF_LORA_PACKET_EXPLICIT                    = RF_LORA_PACKET_VARIABLE_LENGTH,
    RF_LORA_PACKET_IMPLICIT                    = RF_LORA_PACKET_FIXED_LENGTH,
}RF_RadioLoRaPacketLengthsModes_t;


/*!
 * \brief Represents the CRC mode for LORA packet type
 */
typedef enum
{
    RF_LORA_CRC_ON                             = 0x20,         //!< CRC activated
    RF_LORA_CRC_OFF                            = 0x00,         //!< CRC not used
}RF_RadioLoRaCrcModes_t;

/*!
 * \brief Represents the IQ mode for LORA packet type
 */
typedef enum
{
    RF_LORA_IQ_NORMAL                          = 0x40,
    RF_LORA_IQ_INVERTED                        = 0x00,
}RF_RadioLoRaIQModes_t;



/*!
 * \brief Represents the preamble length values for GFSK and FLRC packet
 *        types
 */
typedef enum
{
    RF_PREAMBLE_LENGTH_04_BITS                 = 0x00,         //!< Preamble length: 04 bits
    RF_PREAMBLE_LENGTH_08_BITS                 = 0x10,         //!< Preamble length: 08 bits
    RF_PREAMBLE_LENGTH_12_BITS                 = 0x20,         //!< Preamble length: 12 bits
    RF_PREAMBLE_LENGTH_16_BITS                 = 0x30,         //!< Preamble length: 16 bits
    RF_PREAMBLE_LENGTH_20_BITS                 = 0x40,         //!< Preamble length: 20 bits
    RF_PREAMBLE_LENGTH_24_BITS                 = 0x50,         //!< Preamble length: 24 bits
    RF_PREAMBLE_LENGTH_28_BITS                 = 0x60,         //!< Preamble length: 28 bits
    RF_PREAMBLE_LENGTH_32_BITS                 = 0x70,         //!< Preamble length: 32 bits
}RF_RadioPreambleLengths_t;


/*!
 * \brief The length of sync words for GFSK packet type
 */
typedef enum
{
    RF_GFSK_SYNCWORD_LENGTH_1_BYTE             = 0x00,         //!< Sync word length: 1 byte
    RF_GFSK_SYNCWORD_LENGTH_2_BYTE             = 0x02,         //!< Sync word length: 2 bytes
    RF_GFSK_SYNCWORD_LENGTH_3_BYTE             = 0x04,         //!< Sync word length: 3 bytes
    RF_GFSK_SYNCWORD_LENGTH_4_BYTE             = 0x06,         //!< Sync word length: 4 bytes
    RF_GFSK_SYNCWORD_LENGTH_5_BYTE             = 0x08,         //!< Sync word length: 5 bytes
}RF_RadioSyncWordLengths_t;

/*!
 * \brief Represents the possible combinations of SyncWord correlators
 *        activated for GFSK and FLRC packet types
 */
typedef enum
{
    RF_RADIO_RX_MATCH_SYNCWORD_OFF             = 0x00,         //!< No correlator turned on, i.e. do not search for SyncWord
    RF_RADIO_RX_MATCH_SYNCWORD_1               = 0x10,
    RF_RADIO_RX_MATCH_SYNCWORD_2               = 0x20,
    RF_RADIO_RX_MATCH_SYNCWORD_1_2             = 0x30,
    RF_RADIO_RX_MATCH_SYNCWORD_3               = 0x40,
    RF_RADIO_RX_MATCH_SYNCWORD_1_3             = 0x50,
    RF_RADIO_RX_MATCH_SYNCWORD_2_3             = 0x60,
    RF_RADIO_RX_MATCH_SYNCWORD_1_2_3           = 0x70,
}RF_RadioSyncWordRxMatchs_t;

/*!
 *  \brief Radio packet length mode for GFSK and FLRC packet types
 */
typedef enum
{
    RF_RADIO_PACKET_FIXED_LENGTH               = 0x00,         //!< The packet is known on both sides, no header included in the packet
    RF_RADIO_PACKET_VARIABLE_LENGTH            = 0x20,         //!< The packet is on variable size, header included
}RF_RadioPacketLengthModes_t;

/*!
 * \brief Represents the CRC length for GFSK and FLRC packet types
 *
 * \warning Not all configurations are available for both GFSK and FLRC
 *          packet type. Refer to the datasheet for possible configuration.
 */
typedef enum
{
    RF_RADIO_CRC_OFF                           = 0x00,         //!< No CRC in use
    RF_RADIO_CRC_1_BYTES                       = 0x10,
    RF_RADIO_CRC_2_BYTES                       = 0x20,
    RF_RADIO_CRC_3_BYTES                       = 0x30,
}RF_RadioCrcTypes_t;

/*!
 * \brief Radio whitening mode activated or deactivated for GFSK, FLRC and
 *        BLE packet types
 */
typedef enum
{
    RF_RADIO_WHITENING_ON                      = 0x00,
    RF_RADIO_WHITENING_OFF                     = 0x08,
}RF_RadioWhiteningModes_t;

/*!
 * \brief Represents the SyncWord length for FLRC packet type
 */
typedef enum
{
    RF_FLRC_NO_SYNCWORD                       = 0x00,
    RF_FLRC_SYNCWORD_LENGTH_4_BYTE            = 0x04,
}RF_RadioFlrcSyncWordLengths_t;


/*!
 * \brief Represents the connection state for BLE packet type
 */
typedef enum
{
    RF_BLE_MASTER_SLAVE                        = 0x00,
    RF_BLE_ADVERTISER                          = 0x20,
    RF_BLE_TX_TEST_MODE                        = 0x40,
    RF_BLE_RX_TEST_MODE                        = 0x60,
    RF_BLE_RXTX_TEST_MODE                      = 0x80,
}RF_RadioBleConnectionStates_t;

/*!
 * \brief Represents the CRC field length for BLE packet type
 */
typedef enum
{
    RF_BLE_CRC_OFF                             = 0x00,
    RF_BLE_CRC_3B                              = 0x10,
}RF_RadioBleCrcTypes_t;

/*!
 * \brief Represents the specific packets to use in BLE packet type
 */
typedef enum
{
    RF_BLE_PRBS_9                              = 0x00,         //!< Pseudo Random Binary Sequence based on 9th degree polynomial
    RF_BLE_PRBS_15                             = 0x0C,         //!< Pseudo Random Binary Sequence based on 15th degree polynomial
    RF_BLE_EYELONG_1_0                         = 0x04,         //!< Repeated '11110000' sequence
    RF_BLE_EYELONG_0_1                         = 0x18,         //!< Repeated '00001111' sequence
    RF_BLE_EYESHORT_1_0                        = 0x08,         //!< Repeated '10101010' sequence
    RF_BLE_EYESHORT_0_1                        = 0x1C,         //!< Repeated '01010101' sequence
    RF_BLE_ALL_1                               = 0x10,         //!< Repeated '11111111' sequence
    RF_BLE_ALL_0                               = 0x14,         //!< Repeated '00000000' sequence
}RF_RadioBleTestPayloads_t;



/*!
 * \brief Represents the possible combinations of bitrate and bandwidth for
 *        GFSK and BLE packet types
 *
 * The bitrate is expressed in Mb/s and the bandwidth in MHz
 */
typedef enum
{
    RF_GFSK_BLE_BR_2_000_BW_2_4                = 0x04,
    RF_GFSK_BLE_BR_1_600_BW_2_4                = 0x28,
    RF_GFSK_BLE_BR_1_000_BW_2_4                = 0x4C,
    RF_GFSK_BLE_BR_1_000_BW_1_2                = 0x45,
    RF_GFSK_BLE_BR_0_800_BW_2_4                = 0x70,
    RF_GFSK_BLE_BR_0_800_BW_1_2                = 0x69,
    RF_GFSK_BLE_BR_0_500_BW_1_2                = 0x8D,
    RF_GFSK_BLE_BR_0_500_BW_0_6                = 0x86,
    RF_GFSK_BLE_BR_0_400_BW_1_2                = 0xB1,
    RF_GFSK_BLE_BR_0_400_BW_0_6                = 0xAA,
    RF_GFSK_BLE_BR_0_250_BW_0_6                = 0xCE,
    RF_GFSK_BLE_BR_0_250_BW_0_3                = 0xC7,
    RF_GFSK_BLE_BR_0_125_BW_0_3                = 0xEF,
}RF_RadioGfskBleBitrates_t;

/*!
 * \brief Represents the modulation index used in GFSK and BLE packet
 *        types
 */
typedef enum
{
    RF_GFSK_BLE_MOD_IND_0_35                   =  0,
    RF_GFSK_BLE_MOD_IND_0_50                   =  1,
    RF_GFSK_BLE_MOD_IND_0_75                   =  2,
    RF_GFSK_BLE_MOD_IND_1_00                   =  3,
    RF_GFSK_BLE_MOD_IND_1_25                   =  4,
    RF_GFSK_BLE_MOD_IND_1_50                   =  5,
    RF_GFSK_BLE_MOD_IND_1_75                   =  6,
    RF_GFSK_BLE_MOD_IND_2_00                   =  7,
    RF_GFSK_BLE_MOD_IND_2_25                   =  8,
    RF_GFSK_BLE_MOD_IND_2_50                   =  9,
    RF_GFSK_BLE_MOD_IND_2_75                   = 10,
    RF_GFSK_BLE_MOD_IND_3_00                   = 11,
    RF_GFSK_BLE_MOD_IND_3_25                   = 12,
    RF_GFSK_BLE_MOD_IND_3_50                   = 13,
    RF_GFSK_BLE_MOD_IND_3_75                   = 14,
    RF_GFSK_BLE_MOD_IND_4_00                   = 15,
}RF_RadioGfskBleModIndexes_t;

/*!
 * \brief Represents the modulation shaping parameter for GFSK, FLRC and BLE
 *        packet types
 */
typedef enum
{
    RF_RADIO_MOD_SHAPING_BT_OFF                = 0x00,         //! No filtering
    RF_RADIO_MOD_SHAPING_BT_1_0                = 0x10,
    RF_RADIO_MOD_SHAPING_BT_0_5                = 0x20,
}RF_RadioModShapings_t;


/*!
 * \brief Represents the possible combination of bitrate and bandwidth for FLRC
 *        packet type
 *
 * The bitrate is in Mb/s and the bitrate in MHz
 */
typedef enum
{
    RF_FLRC_BR_1_300_BW_1_2                    = 0x45,
    RF_FLRC_BR_1_040_BW_1_2                    = 0x69,
    RF_FLRC_BR_0_650_BW_0_6                    = 0x86,
    RF_FLRC_BR_0_520_BW_0_6                    = 0xAA,
    RF_FLRC_BR_0_325_BW_0_3                    = 0xC7,
    RF_FLRC_BR_0_260_BW_0_3                    = 0xEB,
}RF_RadioFlrcBitrates_t;

/*!
 * \brief Represents the possible values for coding rate parameter in FLRC
 *        packet type
 */
typedef enum
{
    RF_FLRC_CR_1_2                             = 0x00,
    RF_FLRC_CR_3_4                             = 0x02,
    RF_FLRC_CR_1_0                             = 0x04,
}RF_RadioFlrcCodingRates_t;


/*!
 * \brief Represents the interruption masks available for the radio
 *
 * \remark Note that not all these interruptions are available for all packet types
 */
typedef enum
{
    RF_IRQ_RADIO_NONE                          = 0x0000,
    RF_IRQ_TX_DONE                             = 0x0001,
    RF_IRQ_RX_DONE                             = 0x0002,
    RF_IRQ_SYNCWORD_VALID                      = 0x0004,
    RF_IRQ_SYNCWORD_ERROR                      = 0x0008,
    RF_IRQ_HEADER_VALID                        = 0x0010,
    RF_IRQ_HEADER_ERROR                        = 0x0020,
    RF_IRQ_CRC_ERROR                           = 0x0040,
    RF_IRQ_RANGING_SLAVE_RESPONSE_DONE         = 0x0080,
    RF_IRQ_RANGING_SLAVE_REQUEST_DISCARDED     = 0x0100,
    RF_IRQ_RANGING_MASTER_RESULT_VALID         = 0x0200,
    RF_IRQ_RANGING_MASTER_RESULT_TIMEOUT       = 0x0400,
    RF_IRQ_RANGING_SLAVE_REQUEST_VALID         = 0x0800,
    RF_IRQ_CAD_DONE                            = 0x1000,
    RF_IRQ_CAD_ACTIVITY_DETECTED               = 0x2000,
    RF_IRQ_RX_TX_TIMEOUT                       = 0x4000,
    RF_IRQ_PREAMBLE_DETECTED                   = 0x8000,
    RF_IRQ_RADIO_ALL                           = 0xFFFF,
}RF_RadioIrqMasks_t;


/*!
 * \brief Represents the tick size available for Rx/Tx timeout operations
 */
typedef enum
{
    RF_RADIO_TICK_SIZE_0015_US                 = 0x00,
    RF_RADIO_TICK_SIZE_0062_US                 = 0x01,
    RF_RADIO_TICK_SIZE_1000_US                 = 0x02,
    RF_RADIO_TICK_SIZE_4000_US                 = 0x03,
}RF_RadioTickSizes_t;


/*!
 * \brief Status of ranging distance
 */
enum RangingStatus
{
    RANGING_INIT = 0,
    RANGING_PROCESS,
    RANGING_VALID,
    RANGING_TIMEOUT,
    RANGING_PER_ERROR
};


#endif
