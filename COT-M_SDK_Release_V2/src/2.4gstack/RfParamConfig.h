/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2016 Semtech

Description: rf_param routines header

Maintainer: Gregory Cristian & Gilbert Menth
*/

#ifndef RF_PARAM_CONFIG_H
#define RF_PARAM_CONFIG_H


#include <stdint.h>
#include "cot_mac.h"
#include "cot_ranging.h"

/*!
 * \brief Define Buffer indexes for each ConfigData for copy and restore
 */
#define SCR_CAL_FLAG_RF_PARAM_ADDR            0   // boolean
#define SCR_CAL_POSA_RF_PARAM_ADDR            1   // int32
#define SCR_CAL_POSB_RF_PARAM_ADDR            5   // int32
#define SCR_CAL_POSC_RF_PARAM_ADDR            9   // int32
#define SCR_CAL_POSD_RF_PARAM_ADDR            13  // int32
#define SCR_CAL_POSE_RF_PARAM_ADDR            17  // int32
#define SCR_CAL_POSF_RF_PARAM_ADDR            21  // int32
#define APP_ENTITY_RF_PARAM_ADDR              25  // uint8
#define APP_ANT_SW_RF_PARAM_ADDR              26  // uint8
#define APP_FREQ_RF_PARAM_ADDR                27  // uint32_t Frequency;
#define APP_TXPWR_RF_PARAM_ADDR               31  // int8_t TxPower;
#define APP_MOD_TYPE_RF_PARAM_ADDR            32  // uint8_t ModulationType;
#define APP_PER_NPAK_MAX_RF_PARAM_ADDR        33  // uint32_t MaxNumPacket;
#define APP_RADIO_POWER_MODE_RF_PARAM_ADDR    37  // RadioRegulatorMode_t
#define APP_RNG_REQ_COUNT_RF_PARAM_ADDR       38  // uint8_t
#define APP_RNG_FULLSCALE_RF_PARAM_ADDR       39  // uint16_t
#define APP_RNG_ZSCORE_RF_PARAM_ADDR          41
#define APP_RNG_UNIT_RF_PARAM_ADDR            42  // ranging distance unit
#define APP_RNG_ADDR_RF_PARAM_ADDR            43  // uint32_t
#define APP_RNG_ANT_RF_PARAM_ADDR             47  // uint8_t RngAntenna
#define MOD_PAK_TYPE_RF_PARAM_ADDR            48  // enum
#define MOD_GFS_BRBW_RF_PARAM_ADDR            49  // BitrateBandwidth
#define MOD_GFS_MOD_IND_RF_PARAM_ADDR         50  // ModulationIndex
#define MOD_GFS_MOD_SHAP_RF_PARAM_ADDR        51  // ModulationShaping
#define MOD_LOR_SPREADF_RF_PARAM_ADDR         52  // SpreadingFactor
#define MOD_LOR_BW_RF_PARAM_ADDR              53  // Bandwidth
#define MOD_LOR_CODERATE_RF_PARAM_ADDR        54  // CodingRate
#define MOD_FLR_BRBW_RF_PARAM_ADDR            55  // BitrateBandwidth
#define MOD_FLR_CODERATE_RF_PARAM_ADDR        56  // CodingRate
#define MOD_FLR_MOD_SHAP_RF_PARAM_ADDR        57  // ModulationShaping
#define MOD_BLE_BRBW_RF_PARAM_ADDR            58  // BitrateBandwidth
#define MOD_BLE_MOD_IND_RF_PARAM_ADDR         59  // ModulationIndex
#define MOD_BLE_MOD_SHAP_RF_PARAM_ADDR        60  // ModulationShaping
#define MOD_RNG_SPREADF_RF_PARAM_ADDR         61  // SpreadingFactor
#define MOD_RNG_BW_RF_PARAM_ADDR              62  // Bandwidth
#define MOD_RNG_CODERATE_RF_PARAM_ADDR        63  // CodingRate
#define PAK_PAK_TYPE_RF_PARAM_ADDR            64  //
#define PAK_GFS_PREAMBLE_LEN_RF_PARAM_ADDR    65  // PreambleLength
#define PAK_GFS_SYNC_LEN_RF_PARAM_ADDR        66  // SyncWordLength
#define PAK_GFS_SYNC_MATCH_RF_PARAM_ADDR      67  // SyncWordMatch
#define PAK_GFS_HEADERTYPE_RF_PARAM_ADDR      68  // HeaderType
#define PAK_GFS_PL_LEN_RF_PARAM_ADDR          69  // PayloadLength
#define PAK_GFS_CRC_LEN_RF_PARAM_ADDR         70  // CrcLength
#define PAK_GFS_WHITENING_RF_PARAM_ADDR       71  // Whitening
#define PAK_LOR_PREAMBLE_LEN_RF_PARAM_ADDR    72  // PreambleLength
#define PAK_LOR_HEADERTYPE_RF_PARAM_ADDR      73  // HeaderType
#define PAK_LOR_PL_LEN_RF_PARAM_ADDR          74  // PayloadLength
#define PAK_LOR_CRC_MODE_RF_PARAM_ADDR        75  // CrcMode
#define PAK_LOR_IQ_INV_RF_PARAM_ADDR          76  // InvertIQ
#define PAK_RNG_PREAMBLE_LEN_RF_PARAM_ADDR    77  // PreambleLength
#define PAK_RNG_HEADERTYPE_RF_PARAM_ADDR      78  // HeaderType
#define PAK_RNG_PL_LEN_RF_PARAM_ADDR          79  // PayloadLength
#define PAK_RNG_CRC_MODE_RF_PARAM_ADDR        80  // CrcMode
#define PAK_RNG_IQ_INV_RF_PARAM_ADDR          81  // InvertIQ
#define PAK_FLR_PREAMBLE_LEN_RF_PARAM_ADDR    82  // PreambleLength
#define PAK_FLR_SYNC_LEN_RF_PARAM_ADDR        83  // SyncWordLength
#define PAK_FLR_SYNC_MATCH_RF_PARAM_ADDR      84  // SyncWordMatch
#define PAK_FLR_HEADERTYPE_RF_PARAM_ADDR      85  // HeaderType
#define PAK_FLR_PL_LEN_RF_PARAM_ADDR          86  // PayloadLength
#define PAK_FLR_CRC_LEN_RF_PARAM_ADDR         87  // CrcLength
#define PAK_FLR_WHITENING_RF_PARAM_ADDR       88  // Whitening
#define PAK_BLE_CON_STATE_RF_PARAM_ADDR       89  // ConnectionState
#define PAK_BLE_CRC_FIELD_RF_PARAM_ADDR       90  // CrcField
#define PAK_BLE_PAK_TYPE_RF_PARAM_ADDR        91  // BlePacketType
#define PAK_BLE_WHITENING_RF_PARAM_ADDR       92  // Whitening
#define EEPROM_CRC_EEPROM_ADDR                93  // uint16
#define PAK_COT_DEV_ID                        95  // device id
#define PAK_COT_MULTICAST_ID                  99  // multicast id
#define PAK_COT_BROADCAST_ID                  103 // broadcast id
#define PAK_CHANNEL_COUNT                     107 // communication channel sequence
#define PAK_PARAM_DATA_SET_TYPE_ADDR          115 // uint8
#define RF_PARAM_CRC_RF_PARAM_ADDR            116 // uint16


/*!
 * \brief rf_param buffer size. Cf. above.
 */
#define RF_PARAM_BUFFER_SIZE                  118


/*!
 * \brief Part of rf_param to save or restore
 */
typedef enum
{
    ALL_DATA,
    SCREEN_DATA,
    DEMO_SETTINGS,
    RADIO_LORA_PARAMS,
    RADIO_RANGING_PARAMS,
    RADIO_FLRC_PARAMS,
    RADIO_GFSK_PARAMS,
    RADIO_BLE_PARAMS
}ConfigDataSet_t;



/*!
 * \brief Define min and max payload length for demo applications
 */
#define DEMO_MIN_PAYLOAD            12
#define DEMO_FLRC_MAX_PAYLOAD       127
#define DEMO_GFS_LORA_MAX_PAYLOAD   255

/*!
 * \brief Define range of central frequency [Hz]
 */
#define DEMO_CENTRAL_FREQ_MIN       2400000000UL
#define DEMO_CENTRAL_FREQ_MAX       2483500000UL

/*!
 * \brief Define 3 preset central frequencies [Hz]
 */
#define DEMO_CENTRAL_FREQ_PRESET1   2402000000UL
#define DEMO_CENTRAL_FREQ_PRESET2   2450000000UL
#define DEMO_CENTRAL_FREQ_PRESET3   2480000000UL


/*!
 * \brief Demo Settings structure of rf_param structure
 */
#pragma pack(1)
typedef struct
{
    uint8_t Entity;              // Master or Slave
    uint8_t HoldDemo;            // Put demo in hold status
    uint8_t AntennaSwitch;       // Witch antenna connected
    uint32_t Frequency;          // Demo frequency
    int8_t TxPower;              // Demo Tx power
    uint8_t RadioPowerMode;      // Radio Power Mode [0: LDO, 1:DC_DC]
    uint8_t PayloadLength;       // Demo payload length
    uint8_t ModulationType;      // Demo modulation type (LORA, GFSK, FLRC)
    uint8_t ModulationParam1;    // Demo Mod. Param1 (depend on modulation type)
    uint8_t ModulationParam2;    // Demo Mod. Param2 (depend on modulation type)
    uint8_t ModulationParam3;    // Demo Mod. Param3 (depend on modulation type)
    uint8_t PacketParam1;        // Demo Pack. Param1 (depend on packet type)
    uint8_t PacketParam2;        // Demo Pack. Param2 (depend on packet type)
    uint8_t PacketParam3;        // Demo Pack. Param3 (depend on packet type)
    uint8_t PacketParam4;        // Demo Pack. Param4 (depend on packet type)
    uint8_t PacketParam5;        // Demo Pack. Param5 (depend on packet type)
    uint8_t PacketParam6;        // Demo Pack. Param6 (depend on packet type)
    uint8_t PacketParam7;        // Demo Pack. Param7 (depend on packet type)
    uint32_t MaxNumPacket;       // Demo Max Num Packet for PingPong and PER
    uint16_t InterPacketDelay;   // Demo Inter-Packet Delay for PingPong and PER
    uint32_t CntPacketTx;        // Tx packet transmitted
    uint32_t CntPacketRxOK;      // Rx packet received OK
    uint32_t CntPacketRxOKSlave; // Rx packet received OK (slave side)
    uint32_t CntPacketRxKO;      // Rx packet received KO
    uint32_t CntPacketRxKOSlave; // Rx packet received KO (slave side)
    uint16_t RxTimeOutCount;     // Rx packet received KO (by timeout)
    double RngDistance;          // Distance measured by ranging demo
    uint32_t RngAddress;         // Ranging Address
    uint16_t RngFullScale;       // Full range of measuring distance (Ranging)
    uint8_t RngRequestCount;     // Ranging Request Count
    uint8_t RngUnit;             // Ranging distance unit [m]/[mi]
    uint8_t RngStatus;           // Status of ranging distance
    double RngFei;               // Ranging Frequency Error Indicator
    uint8_t RngAntenna;          // Ranging antenna selection
    double RngFeiFactor;         // Ranging frequency correction factor
    uint16_t RngReqDelay;        // Time between ranging request
    uint16_t RngCalib;           // Ranging Calibration
    uint8_t RngZscoreMax;        // Max Zscore for ranging value ( ! *10 ! )
    int8_t RssiValue;            // Demo Rssi Value
    int8_t SnrValue;             // Demo Snr Value (only for LORA mod. type)
	uint16_t crc;				 // crc
    uint32_t cot_dev_id;          //device id
    uint32_t cot_multicast_id;    //multicast id
    uint32_t cot_broadcast_id;    //broadcast id
    uint64_t ChannelsCount;         // communication channel sequence
    uint8_t  DataSetType;           //data set type
}RFParamSettings_t;
#pragma pack()

/*!
 * \brief ConfigData structure
 */
typedef struct
{
    RFParamSettings_t RFParamSettings;
    PHYModulationParams_t ModulationParams;
    PHYPacketParams_t PacketParams;
    uint16_t CheckSum;
}ConfigData_t;

/*!
 * \brief rf_param structure
 */
typedef struct
{
    ConfigData_t ConfigData;
    // Allows for the checksum to be carried out
    uint8_t Buffer[RF_PARAM_BUFFER_SIZE];
}Rf_Param_t;


/*!
 * \brief Initialises the contents of ConfigData
 */
void DataFlashInit(ConfigData_t *ConfigData);

/*!
 * \brief Read rf_param from emulated rf_param (in fact in Flash " higher address).
 *
 * \param [in]  addr          address of data (rf_param offset not to be include)
 * \param [in]  buffer        buffer to use for copy
 * \param [in]  size          size of data to copy
 *
 * \retval      status        Status of operation (SUCCESS, ..)
 */
uint8_t DataFlashMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * \brief Writes the ConfigData to emulated rf_param
 *
 * \param [in]  dataSet       Set of data to save or restore
 * \param [in]  save          true  save to flash
 */
void DataFlashSaveSettings(uint8_t *buffer,ConfigData_t *configdata,bool save,RF_RadioPacketTypes_t dataSet);

/*!
 * \brief Loads ConfigData from emulated rf_param
 */
void DataFlashLoadGeneralSettings (uint8_t *buffer,ConfigData_t *ConfigData);

/*!
 * \brief Loads ConfigData with updated modulation and packet parameters
 *
 * \param [in]  modulation    modulation type to select for mod. & packet params
 */
void DataFlashLoadSettings(uint8_t *buffer,ConfigData_t *configdata);

/*!
 * \brief Initialises the contents of flash to default values & save to rf_param
 */
void DataFlashSetDefaultSettings( uint8_t *buffer ,ConfigData_t *configdata);

/**
* @name: sample_rf_param_set
* This funtion is to initialization rf param
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-10
*/
int32_t sample_rf_param_get(ConfigData_t ConfigData, PHYParamSettings_t *phy_param_setting );


/**
* @name: get_rf_param
* This funtion is to get rf params
* @param[in]   rf_config_param -- the point of rf params.
* @param[out]  rf_config_param -- the point of rf params
* @retval  ERR_SUCCESS 0
* @retval  ERR_FAIL < 0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-10
*/
int32_t get_rf_param( ConfigData_t *rf_config_param );


/**
* @name: set_rf_param
* This funtion is to set rf params
* @param[in]   g_rf_param -- the point of rf params.
* @param[in]   set_to_flah_flag -- the flag to write to flash
* @param[out]  none
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-10
*/
int32_t set_rf_param( ConfigData_t *rf_config_param,bool set_to_flah_flag);


/**
* @name: get_dev_id
* This funtion is to get device id
* @param[in]   none
* @param[in]   none
* @param[out]  none
* @retval  device id
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-10
*/
uint32_t get_dev_id(void);


#endif //rf_param_H
