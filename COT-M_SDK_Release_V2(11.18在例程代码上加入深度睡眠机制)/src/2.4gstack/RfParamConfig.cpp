/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2016 Semtech

Description: rf_param routines

Maintainer: Gregory Cristian & Gilbert Menth
*/

//#include "mbed.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "am_bsp.h"
#include "error.h"
#include "vfs.h"
#include "flash.h"

#include "RfParamConfig.h"
#include "log.h"


/*********************************************************************
 * MACROS
 */
#define MSG(FORMAT, ARG...) /* message that is destined to the user */
#define MSG_DEBUG(FLAG, fmt, ...)                                                                         \
            do  {                                                                                         \
                if (FLAG)                                                                                 \
                    log_printf("%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
            } while (0)

#define TARGET_APOLLO_BGA_512


/*!
 * \brief Define address of Emulated rf_param (in fact region of Flash)
 */
#if defined( TARGET_NUCLEO_L476RG )
#define DATA_RF_PARAM_BASE    ( ( uint32_t )0x0807F800U )
#define DATA_RF_PARAM_END     ( ( uint32_t )DATA_RF_PARAM_BASE + 2048 )
#elif defined( TARGET_NUCLEO_L152RE )
#define DATA_RF_PARAM_BASE    ( ( uint32_t )0x08080000U )
#define DATA_RF_PARAM_END     ( ( uint32_t )0x080807FFU )
#elif defined( TARGET_APOLLO_BGA_512 )
#define DATA_RF_PARAM_BASE    ( ( uint32_t )0x0007C800U )
#define DATA_RF_PARAM_END     ( ( uint32_t )0x0007CFFFU )
#else
#error "Please define g_rf_param base address and size for your board "
#endif

#define assert_param( ... )

/*!
 * \brief CRC of rf_param buffer and its valid status
 */
typedef struct
{
    uint16_t Value;
    bool Valid;
}MemTestStruct_t;

/*!
 * \brief Local copy of rf_param.
 */
//Rf_Param_t g_rf_param;




// Check CRC of local copy of rf_param (Buffer). This update Valid & Value
static MemTestStruct_t DataFlashDataCheckSum( uint8_t *buffer );
uint8_t DataFlashMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size );

static uint32_t LoRaMAC_BKDRHash(uint8_t *str);



/**
* @Description: 获取设备ID
* @return 设备ID
*/
static uint32_t LoRaMAC_IDHashProgram(void);


void DataFlashInit(ConfigData_t *configdata)
{
    MemTestStruct_t memTestStruct;
    uint8_t buffer[RF_PARAM_BUFFER_SIZE] = {0};

    DataFlashMcuReadBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );
    DataFlashLoadGeneralSettings(buffer,configdata);

    memTestStruct = DataFlashDataCheckSum(buffer);
    if( !( memTestStruct.Valid ) )
	{
        //MSG_DEBUG(1,"ConfigDataCheckSum failed\n\r");
        DataFlashSetDefaultSettings(buffer,configdata);
    }

	DataFlashLoadSettings(buffer,configdata);
}



void DataFlashSaveSettings(uint8_t *buffer,ConfigData_t *configdata,bool save, RF_RadioPacketTypes_t dataSet)
{
    MemTestStruct_t memTestStruct;

    switch( dataSet )
    {
        case RF_PACKET_TYPE_LORA:
            configdata->ModulationParams.Params.LoRa.SpreadingFactor = ( RF_RadioLoRaSpreadingFactors_t )  configdata->RFParamSettings.ModulationParam1;
            configdata->ModulationParams.Params.LoRa.Bandwidth       = ( RF_RadioLoRaBandwidths_t )        configdata->RFParamSettings.ModulationParam2;
            configdata->ModulationParams.Params.LoRa.CodingRate      = ( RF_RadioLoRaCodingRates_t )       configdata->RFParamSettings.ModulationParam3;
            configdata->PacketParams.Params.LoRa.PreambleLength      =                                     configdata->RFParamSettings.PacketParam1;
            configdata->PacketParams.Params.LoRa.HeaderType          = ( RF_RadioLoRaPacketLengthsModes_t )configdata->RFParamSettings.PacketParam2;
            configdata->PacketParams.Params.LoRa.PayloadLength       =                                     configdata->RFParamSettings.PacketParam3;
            configdata->PacketParams.Params.LoRa.Crc                 = ( RF_RadioLoRaCrcModes_t )          configdata->RFParamSettings.PacketParam4;
            configdata->PacketParams.Params.LoRa.InvertIQ            = ( RF_RadioLoRaIQModes_t )           configdata->RFParamSettings.PacketParam5;

            if(buffer != NULL){
                memcpy( buffer + MOD_LOR_SPREADF_RF_PARAM_ADDR,      &( configdata->ModulationParams.Params.LoRa.SpreadingFactor ), 1 );
                memcpy( buffer + MOD_LOR_BW_RF_PARAM_ADDR,           &( configdata->ModulationParams.Params.LoRa.Bandwidth ),       1 );
                memcpy( buffer + MOD_LOR_CODERATE_RF_PARAM_ADDR,     &( configdata->ModulationParams.Params.LoRa.CodingRate ),      1 );
                memcpy( buffer + PAK_LOR_PREAMBLE_LEN_RF_PARAM_ADDR, &( configdata->PacketParams.Params.LoRa.PreambleLength ),      1 );
                memcpy( buffer + PAK_LOR_HEADERTYPE_RF_PARAM_ADDR,   &( configdata->PacketParams.Params.LoRa.HeaderType ),          1 );
                memcpy( buffer + PAK_LOR_PL_LEN_RF_PARAM_ADDR,       &( configdata->PacketParams.Params.LoRa.PayloadLength ),       1 );
                memcpy( buffer + PAK_LOR_CRC_MODE_RF_PARAM_ADDR,     &( configdata->PacketParams.Params.LoRa.Crc ),             1 );
                memcpy( buffer + PAK_LOR_IQ_INV_RF_PARAM_ADDR,       &( configdata->PacketParams.Params.LoRa.InvertIQ ),            1 );
            }//MSG_DEBUG(0,"Saved RADIO_LORA_PARAMS\n\r");
            break;

        case RF_PACKET_TYPE_RANGING:

            configdata->ModulationParams.Params.Rng.SpreadingFactor = ( RF_RadioLoRaSpreadingFactors_t )  configdata->RFParamSettings.ModulationParam1;
            configdata->ModulationParams.Params.Rng.Bandwidth       = ( RF_RadioLoRaBandwidths_t )        configdata->RFParamSettings.ModulationParam2;
            configdata->ModulationParams.Params.Rng.CodingRate      = ( RF_RadioLoRaCodingRates_t )       configdata->RFParamSettings.ModulationParam3;
            configdata->PacketParams.Params.Rng.PreambleLength      =                                     configdata->RFParamSettings.PacketParam1;
            configdata->PacketParams.Params.Rng.HeaderType          = ( RF_RadioLoRaPacketLengthsModes_t )configdata->RFParamSettings.PacketParam2;
            configdata->PacketParams.Params.Rng.PayloadLength       =                                     configdata->RFParamSettings.PacketParam3;
            configdata->PacketParams.Params.Rng.Crc             	= ( RF_RadioLoRaCrcModes_t )          configdata->RFParamSettings.PacketParam4;
            configdata->PacketParams.Params.Rng.InvertIQ            = ( RF_RadioLoRaIQModes_t )           configdata->RFParamSettings.PacketParam5;

            configdata->ModulationParams.Params.Rng.RngRequestCount = configdata->RFParamSettings.RngRequestCount;
            configdata->ModulationParams.Params.Rng.RngFullScale =    configdata->RFParamSettings.RngFullScale;
            configdata->ModulationParams.Params.Rng.RngZscoreMax =    configdata->RFParamSettings.RngZscoreMax;
            configdata->ModulationParams.Params.Rng.RngAddress =      configdata->RFParamSettings.RngAddress;
            configdata->ModulationParams.Params.Rng.RngAntenna =      configdata->RFParamSettings.RngAntenna;
            configdata->ModulationParams.Params.Rng.RngUnit =         configdata->RFParamSettings.RngUnit;

            if(buffer != NULL){
                memcpy( buffer + MOD_RNG_SPREADF_RF_PARAM_ADDR,      &( configdata->RFParamSettings.ModulationParam1 ), 1 );
                memcpy( buffer + MOD_RNG_BW_RF_PARAM_ADDR,           &( configdata->RFParamSettings.ModulationParam2 ), 1 );
                memcpy( buffer + MOD_RNG_CODERATE_RF_PARAM_ADDR,     &( configdata->RFParamSettings.ModulationParam3 ), 1 );
                memcpy( buffer + PAK_RNG_PREAMBLE_LEN_RF_PARAM_ADDR, &( configdata->RFParamSettings.PacketParam1 ),     1 );
                memcpy( buffer + PAK_RNG_HEADERTYPE_RF_PARAM_ADDR,   &( configdata->RFParamSettings.PacketParam2 ),     1 );
                memcpy( buffer + PAK_RNG_PL_LEN_RF_PARAM_ADDR,       &( configdata->RFParamSettings.PacketParam3 ),     1 );
                memcpy( buffer + PAK_RNG_CRC_MODE_RF_PARAM_ADDR,     &( configdata->RFParamSettings.PacketParam4 ),     1 );
                memcpy( buffer + PAK_RNG_IQ_INV_RF_PARAM_ADDR,       &( configdata->RFParamSettings.PacketParam5 ),     1 );

                memcpy( buffer + APP_RNG_REQ_COUNT_RF_PARAM_ADDR, &( configdata->RFParamSettings.RngRequestCount ), 1 );
                memcpy( buffer + APP_RNG_FULLSCALE_RF_PARAM_ADDR, &( configdata->RFParamSettings.RngFullScale ),    2 );
                memcpy( buffer + APP_RNG_ZSCORE_RF_PARAM_ADDR,    &( configdata->RFParamSettings.RngZscoreMax ),    1 );
                memcpy( buffer + APP_RNG_ADDR_RF_PARAM_ADDR,      &( configdata->RFParamSettings.RngAddress ),      4 );
                memcpy( buffer + APP_RNG_ANT_RF_PARAM_ADDR,       &( configdata->RFParamSettings.RngAntenna ),      1 );
                memcpy( buffer + APP_RNG_UNIT_RF_PARAM_ADDR,      &( configdata->RFParamSettings.RngUnit ),         1 );

                memcpy( buffer + PAK_CHANNEL_COUNT,               &( configdata->RFParamSettings.ChannelsCount), 8 );
            }//MSG_DEBUG(0,"Saved RADIO_RANGING_PARAMS\n\r");
            break;

        case RF_PACKET_TYPE_FLRC:
            configdata->ModulationParams.Params.Flrc.BitrateBandwidth  = ( RF_RadioFlrcBitrates_t )       configdata->RFParamSettings.ModulationParam1;
            configdata->ModulationParams.Params.Flrc.CodingRate        = ( RF_RadioFlrcCodingRates_t )    configdata->RFParamSettings.ModulationParam2;
            configdata->ModulationParams.Params.Flrc.ModulationShaping = ( RF_RadioModShapings_t )        configdata->RFParamSettings.ModulationParam3;
            configdata->PacketParams.Params.Flrc.PreambleLength        = ( RF_RadioPreambleLengths_t )    configdata->RFParamSettings.PacketParam1;
            configdata->PacketParams.Params.Flrc.SyncWordLength        = ( RF_RadioFlrcSyncWordLengths_t )configdata->RFParamSettings.PacketParam2;
            configdata->PacketParams.Params.Flrc.SyncWordMatch         = ( RF_RadioSyncWordRxMatchs_t )   configdata->RFParamSettings.PacketParam3;
            configdata->PacketParams.Params.Flrc.HeaderType            = ( RF_RadioPacketLengthModes_t )  configdata->RFParamSettings.PacketParam4;
            configdata->PacketParams.Params.Flrc.PayloadLength         =                               	configdata->RFParamSettings.PacketParam5;
            configdata->PacketParams.Params.Flrc.CrcLength             = ( RF_RadioCrcTypes_t )           configdata->RFParamSettings.PacketParam6;
            configdata->PacketParams.Params.Flrc.Whitening             = ( RF_RadioWhiteningModes_t )     configdata->RFParamSettings.PacketParam7;

            if(buffer != NULL){
                memcpy( buffer + MOD_FLR_BRBW_RF_PARAM_ADDR,         &( configdata->ModulationParams.Params.Flrc.BitrateBandwidth ),  1 );
                memcpy( buffer + MOD_FLR_CODERATE_RF_PARAM_ADDR,     &( configdata->ModulationParams.Params.Flrc.CodingRate ),        1 );
                memcpy( buffer + MOD_FLR_MOD_SHAP_RF_PARAM_ADDR,     &( configdata->ModulationParams.Params.Flrc.ModulationShaping ), 1 );
                memcpy( buffer + PAK_FLR_PREAMBLE_LEN_RF_PARAM_ADDR, &( configdata->PacketParams.Params.Flrc.PreambleLength ),        1 );
                memcpy( buffer + PAK_FLR_SYNC_LEN_RF_PARAM_ADDR,     &( configdata->PacketParams.Params.Flrc.SyncWordLength ),        1 );
                memcpy( buffer + PAK_FLR_SYNC_MATCH_RF_PARAM_ADDR,   &( configdata->PacketParams.Params.Flrc.SyncWordMatch ),         1 );
                memcpy( buffer + PAK_FLR_HEADERTYPE_RF_PARAM_ADDR,   &( configdata->PacketParams.Params.Flrc.HeaderType ),            1 );
                memcpy( buffer + PAK_FLR_PL_LEN_RF_PARAM_ADDR,       &( configdata->PacketParams.Params.Flrc.PayloadLength ),         1 );
                memcpy( buffer + PAK_FLR_CRC_LEN_RF_PARAM_ADDR,      &( configdata->PacketParams.Params.Flrc.CrcLength ),             1 );
                memcpy( buffer + PAK_FLR_WHITENING_RF_PARAM_ADDR,    &( configdata->PacketParams.Params.Flrc.Whitening ),             1 );
            }//MSG_DEBUG(0,"Saved RADIO_FLRC_PARAMS\n\r");
            break;

        case RF_PACKET_TYPE_GFSK:
            configdata->ModulationParams.Params.Gfsk.BitrateBandwidth  = ( RF_RadioGfskBleBitrates_t )  configdata->RFParamSettings.ModulationParam1;
            configdata->ModulationParams.Params.Gfsk.ModulationIndex   = ( RF_RadioGfskBleModIndexes_t )configdata->RFParamSettings.ModulationParam2;
            configdata->ModulationParams.Params.Gfsk.ModulationShaping = ( RF_RadioModShapings_t )      configdata->RFParamSettings.ModulationParam3;
            configdata->PacketParams.Params.Gfsk.PreambleLength        = ( RF_RadioPreambleLengths_t )  configdata->RFParamSettings.PacketParam1;
            configdata->PacketParams.Params.Gfsk.SyncWordLength        = ( RF_RadioSyncWordLengths_t )  configdata->RFParamSettings.PacketParam2;
            configdata->PacketParams.Params.Gfsk.SyncWordMatch         = ( RF_RadioSyncWordRxMatchs_t ) configdata->RFParamSettings.PacketParam3;
            configdata->PacketParams.Params.Gfsk.HeaderType            = ( RF_RadioPacketLengthModes_t )configdata->RFParamSettings.PacketParam4;
            configdata->PacketParams.Params.Gfsk.PayloadLength         =                                configdata->RFParamSettings.PacketParam5;
            configdata->PacketParams.Params.Gfsk.CrcLength             = ( RF_RadioCrcTypes_t )         configdata->RFParamSettings.PacketParam6;
            configdata->PacketParams.Params.Gfsk.Whitening             = ( RF_RadioWhiteningModes_t )   configdata->RFParamSettings.PacketParam7;

            if(buffer != NULL){
                memcpy( buffer + MOD_GFS_BRBW_RF_PARAM_ADDR,         &( configdata->ModulationParams.Params.Gfsk.BitrateBandwidth ),  1 );
                memcpy( buffer + MOD_GFS_MOD_IND_RF_PARAM_ADDR,      &( configdata->ModulationParams.Params.Gfsk.ModulationIndex ),   1 );
                memcpy( buffer + MOD_GFS_MOD_SHAP_RF_PARAM_ADDR,     &( configdata->ModulationParams.Params.Gfsk.ModulationShaping ), 1 );
                memcpy( buffer + PAK_GFS_PREAMBLE_LEN_RF_PARAM_ADDR, &( configdata->PacketParams.Params.Gfsk.PreambleLength ),        1 );
                memcpy( buffer + PAK_GFS_SYNC_LEN_RF_PARAM_ADDR,     &( configdata->PacketParams.Params.Gfsk.SyncWordLength ),        1 );
                memcpy( buffer + PAK_GFS_SYNC_MATCH_RF_PARAM_ADDR,   &( configdata->PacketParams.Params.Gfsk.SyncWordMatch ),         1 );
                memcpy( buffer + PAK_GFS_HEADERTYPE_RF_PARAM_ADDR,   &( configdata->PacketParams.Params.Gfsk.HeaderType ),            1 );
                memcpy( buffer + PAK_GFS_PL_LEN_RF_PARAM_ADDR,       &( configdata->PacketParams.Params.Gfsk.PayloadLength ),         1 );
                memcpy( buffer + PAK_GFS_CRC_LEN_RF_PARAM_ADDR,      &( configdata->PacketParams.Params.Gfsk.CrcLength ),             1 );
                memcpy( buffer + PAK_GFS_WHITENING_RF_PARAM_ADDR,    &( configdata->PacketParams.Params.Gfsk.Whitening ),             1 );

            }//MSG_DEBUG(0,"Saved RADIO_GFSK_PARAMS\n\r");
            break;

     default:

            //MSG_DEBUG(0,"Saved DEMO_SETTINGS\n\r");
            //MSG_DEBUG(0,"data not saved\n\r");
            break;
    }
    if(buffer != NULL){
        memcpy( buffer + APP_ENTITY_RF_PARAM_ADDR,           &( configdata->RFParamSettings.Entity ),           1 );
        memcpy( buffer + APP_ANT_SW_RF_PARAM_ADDR,           &( configdata->RFParamSettings.AntennaSwitch ),    1 );
        memcpy( buffer + APP_FREQ_RF_PARAM_ADDR,             &( configdata->RFParamSettings.Frequency ),        4 );
        memcpy( buffer + APP_TXPWR_RF_PARAM_ADDR,            &( configdata->RFParamSettings.TxPower ),          1 );
        memcpy( buffer + APP_MOD_TYPE_RF_PARAM_ADDR,         &( configdata->RFParamSettings.ModulationType ),   1 );
        memcpy( buffer + APP_PER_NPAK_MAX_RF_PARAM_ADDR,     &( configdata->RFParamSettings.MaxNumPacket ),     4 );
        memcpy( buffer + APP_RADIO_POWER_MODE_RF_PARAM_ADDR, &( configdata->RFParamSettings.RadioPowerMode ),   1 );
        memcpy( buffer + MOD_PAK_TYPE_RF_PARAM_ADDR,         &( configdata->RFParamSettings.ModulationType ),   1 );
        memcpy( buffer + PAK_PAK_TYPE_RF_PARAM_ADDR,         &( configdata->RFParamSettings.ModulationType ),   1 );
        memcpy( buffer + PAK_COT_DEV_ID,                     &( configdata->RFParamSettings.cot_dev_id),        4 );
        memcpy( buffer + PAK_COT_MULTICAST_ID,               &( configdata->RFParamSettings.cot_multicast_id),  4 );
        memcpy( buffer + PAK_COT_BROADCAST_ID,               &( configdata->RFParamSettings.cot_broadcast_id),  4 );
        memcpy( buffer + PAK_PARAM_DATA_SET_TYPE_ADDR,       &( configdata->RFParamSettings.DataSetType), 1 );
    }

    if(( save )&&(buffer != NULL))
    {
        memTestStruct = DataFlashDataCheckSum(buffer);
        memcpy( buffer + RF_PARAM_CRC_RF_PARAM_ADDR, &( memTestStruct.Value ), 2 );
        DataFlashMcuWriteBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );
    }


}

void DataFlashLoadGeneralSettings(uint8_t *buffer,ConfigData_t *ConfigData)
{
    //MSG_DEBUG(0,"Load General Settings\n\r");
    memcpy( &( ConfigData->RFParamSettings.Entity ),         buffer + APP_ENTITY_RF_PARAM_ADDR,           1 );
    memcpy( &( ConfigData->RFParamSettings.AntennaSwitch ),  buffer + APP_ANT_SW_RF_PARAM_ADDR,           1 );
    memcpy( &( ConfigData->RFParamSettings.Frequency ),      buffer + APP_FREQ_RF_PARAM_ADDR,             4 );
    memcpy( &( ConfigData->RFParamSettings.RadioPowerMode ), buffer + APP_RADIO_POWER_MODE_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->RFParamSettings.TxPower ),        buffer + APP_TXPWR_RF_PARAM_ADDR,            1 );
    memcpy( &( ConfigData->RFParamSettings.ModulationType ), buffer + APP_MOD_TYPE_RF_PARAM_ADDR,         1 );
    memcpy( &( ConfigData->RFParamSettings.MaxNumPacket ),   buffer + APP_PER_NPAK_MAX_RF_PARAM_ADDR,     4 );

//    memcpy( &( ConfigData->ModulationParams.PacketType ),                    buffer + MOD_PAK_TYPE_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->ModulationParams.Params.Gfsk.BitrateBandwidth ),  buffer + MOD_GFS_BRBW_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->ModulationParams.Params.Gfsk.ModulationIndex ),   buffer + MOD_GFS_MOD_IND_RF_PARAM_ADDR,  1 );
    memcpy( &( ConfigData->ModulationParams.Params.Gfsk.ModulationShaping ), buffer + MOD_GFS_MOD_SHAP_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->ModulationParams.Params.LoRa.SpreadingFactor ),   buffer + MOD_LOR_SPREADF_RF_PARAM_ADDR,  1 );
    memcpy( &( ConfigData->ModulationParams.Params.LoRa.Bandwidth ),         buffer + MOD_LOR_BW_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->ModulationParams.Params.LoRa.CodingRate ),        buffer + MOD_LOR_CODERATE_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->ModulationParams.Params.Flrc.BitrateBandwidth ),  buffer + MOD_FLR_BRBW_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->ModulationParams.Params.Flrc.CodingRate ),        buffer + MOD_FLR_CODERATE_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->ModulationParams.Params.Flrc.ModulationShaping ), buffer + MOD_FLR_MOD_SHAP_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->ModulationParams.Params.Ble.BitrateBandwidth ),   buffer + MOD_BLE_BRBW_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->ModulationParams.Params.Ble.ModulationIndex ),    buffer + MOD_BLE_MOD_IND_RF_PARAM_ADDR,  1 );
    memcpy( &( ConfigData->ModulationParams.Params.Ble.ModulationShaping ),  buffer + MOD_BLE_MOD_SHAP_RF_PARAM_ADDR, 1 );

    //memcpy( &( ConfigData->PacketParams.PacketType ),                 buffer + PAK_PAK_TYPE_RF_PARAM_ADDR,         1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.PreambleLength ), buffer + PAK_GFS_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.SyncWordLength ), buffer + PAK_GFS_SYNC_LEN_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.SyncWordMatch ),  buffer + PAK_GFS_SYNC_MATCH_RF_PARAM_ADDR,   1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.HeaderType ),     buffer + PAK_GFS_HEADERTYPE_RF_PARAM_ADDR,   1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.PayloadLength ),  buffer + PAK_GFS_PL_LEN_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.CrcLength ),      buffer + PAK_GFS_CRC_LEN_RF_PARAM_ADDR,      1 );
    memcpy( &( ConfigData->PacketParams.Params.Gfsk.Whitening ),      buffer + PAK_GFS_WHITENING_RF_PARAM_ADDR,    1 );
    memcpy( &( ConfigData->PacketParams.Params.LoRa.PreambleLength ), buffer + PAK_LOR_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->PacketParams.Params.LoRa.HeaderType ),     buffer + PAK_LOR_HEADERTYPE_RF_PARAM_ADDR,   1 );
    memcpy( &( ConfigData->PacketParams.Params.LoRa.PayloadLength ),  buffer + PAK_LOR_PL_LEN_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->PacketParams.Params.LoRa.Crc ),            buffer + PAK_LOR_CRC_MODE_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->PacketParams.Params.LoRa.InvertIQ ),       buffer + PAK_LOR_IQ_INV_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.PreambleLength ), buffer + PAK_FLR_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.SyncWordLength ), buffer + PAK_FLR_SYNC_LEN_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.SyncWordMatch ),  buffer + PAK_FLR_SYNC_MATCH_RF_PARAM_ADDR,   1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.HeaderType ),     buffer + PAK_FLR_HEADERTYPE_RF_PARAM_ADDR,   1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.PayloadLength ),  buffer + PAK_FLR_PL_LEN_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.CrcLength ),      buffer + PAK_FLR_CRC_LEN_RF_PARAM_ADDR,      1 );
    memcpy( &( ConfigData->PacketParams.Params.Flrc.Whitening ),      buffer + PAK_FLR_WHITENING_RF_PARAM_ADDR,    1 );
    memcpy( &( ConfigData->PacketParams.Params.Ble.BleTestPayload ),   buffer + PAK_BLE_PAK_TYPE_RF_PARAM_ADDR,     1 );
    memcpy( &( ConfigData->PacketParams.Params.Ble.ConnectionState ), buffer + PAK_BLE_CON_STATE_RF_PARAM_ADDR,    1 );
    memcpy( &( ConfigData->PacketParams.Params.Ble.CrcLength  ),        buffer + PAK_BLE_CRC_FIELD_RF_PARAM_ADDR,    1 );
    memcpy( &( ConfigData->PacketParams.Params.Ble.Whitening ),       buffer + PAK_BLE_WHITENING_RF_PARAM_ADDR,    1 );

    memcpy( &( ConfigData->RFParamSettings.RngRequestCount ), buffer + APP_RNG_REQ_COUNT_RF_PARAM_ADDR, 1 );
    memcpy( &( ConfigData->RFParamSettings.RngFullScale ),    buffer + APP_RNG_FULLSCALE_RF_PARAM_ADDR, 2 );
    memcpy( &( ConfigData->RFParamSettings.RngZscoreMax ),    buffer + APP_RNG_ZSCORE_RF_PARAM_ADDR,    1 );
    memcpy( &( ConfigData->RFParamSettings.RngAddress ),      buffer + APP_RNG_ADDR_RF_PARAM_ADDR,      4 );
    memcpy( &( ConfigData->RFParamSettings.RngAntenna ),      buffer + APP_RNG_ANT_RF_PARAM_ADDR,       1 );
    memcpy( &( ConfigData->RFParamSettings.RngUnit ),         buffer + APP_RNG_UNIT_RF_PARAM_ADDR,      1 );

    memcpy( &( ConfigData->RFParamSettings.cot_dev_id ) ,          buffer + PAK_COT_DEV_ID,                  4 );
    memcpy( &( ConfigData->RFParamSettings.cot_multicast_id ),    buffer + PAK_COT_MULTICAST_ID,      4 );
    memcpy( &( ConfigData->RFParamSettings.cot_broadcast_id ),    buffer + PAK_COT_BROADCAST_ID,      4 );
    memcpy( &( ConfigData->RFParamSettings.ChannelsCount ),   buffer + PAK_CHANNEL_COUNT,     8 );
    memcpy( &( ConfigData->RFParamSettings.DataSetType),      buffer + PAK_PARAM_DATA_SET_TYPE_ADDR,1);

	#if 0
	uint32_t dev_id;

	dev_id = (ConfigData->RFParamSettings.cot_dev_id & 0xFF);
	dev_id |= (ConfigData->RFParamSettings.cot_dev_id & 0xFF00);
	dev_id |= (ConfigData->RFParamSettings.cot_dev_id & 0xFF0000);
	dev_id |= (ConfigData->RFParamSettings.cot_dev_id & 0xFF000000);
	ConfigData->RFParamSettings.cot_dev_id = dev_id;
	#endif
	ConfigData->RFParamSettings.cot_dev_id = LoRaMAC_IDHashProgram();//LoRaMAC_IDHashProgram();//0x00000001;//LoRaMAC_IDHashProgram();

}

void DataFlashLoadSettings(uint8_t *buffer,ConfigData_t *configdata)
{
    RF_RadioPacketTypes_t modulation = (RF_RadioPacketTypes_t)configdata->RFParamSettings.ModulationType;
    if( modulation == RF_PACKET_TYPE_LORA )
    {
        //printf("Load Settings PACKET_TYPE_LORA\n\r");
        memcpy( &( configdata->ModulationParams.Params.LoRa.SpreadingFactor ), buffer + MOD_LOR_SPREADF_RF_PARAM_ADDR,      1 );
        memcpy( &( configdata->ModulationParams.Params.LoRa.Bandwidth ),       buffer + MOD_LOR_BW_RF_PARAM_ADDR,           1 );
        memcpy( &( configdata->ModulationParams.Params.LoRa.CodingRate ),      buffer + MOD_LOR_CODERATE_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.LoRa.PreambleLength ),      buffer + PAK_LOR_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
        memcpy( &( configdata->PacketParams.Params.LoRa.HeaderType ),          buffer + PAK_LOR_HEADERTYPE_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.LoRa.PayloadLength ),       buffer + PAK_LOR_PL_LEN_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->PacketParams.Params.LoRa.Crc ),                 buffer + PAK_LOR_CRC_MODE_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.LoRa.InvertIQ ),            buffer + PAK_LOR_IQ_INV_RF_PARAM_ADDR,       1 );

        configdata->RFParamSettings.ModulationType  = RF_PACKET_TYPE_LORA;
        //configdata->ModulationParams.PacketType   = RF_PACKET_TYPE_LORA;
        //configdata->PacketParams.PacketType       = RF_PACKET_TYPE_LORA;
        configdata->RFParamSettings.ModulationParam1 = configdata->ModulationParams.Params.LoRa.SpreadingFactor;
        configdata->RFParamSettings.ModulationParam2 = configdata->ModulationParams.Params.LoRa.Bandwidth;
        configdata->RFParamSettings.ModulationParam3 = configdata->ModulationParams.Params.LoRa.CodingRate;

        configdata->RFParamSettings.PacketParam1 = configdata->PacketParams.Params.LoRa.PreambleLength;
        configdata->RFParamSettings.PacketParam2 = configdata->PacketParams.Params.LoRa.HeaderType;
        configdata->RFParamSettings.PacketParam3 = configdata->PacketParams.Params.LoRa.PayloadLength;
        configdata->RFParamSettings.PacketParam4 = configdata->PacketParams.Params.LoRa.Crc;
        configdata->RFParamSettings.PacketParam5 = configdata->PacketParams.Params.LoRa.InvertIQ;
        configdata->RFParamSettings.PacketParam6 = 0x00;
        configdata->RFParamSettings.PacketParam7 = 0x00;

    }
    else if( modulation == RF_PACKET_TYPE_RANGING )
    {
        //printf("Load Settings PACKET_TYPE_RANGING\n\r");
        memcpy( &( configdata->ModulationParams.Params.Rng.SpreadingFactor ), buffer + MOD_RNG_SPREADF_RF_PARAM_ADDR,      1 );
        memcpy( &( configdata->ModulationParams.Params.Rng.Bandwidth ),       buffer + MOD_RNG_BW_RF_PARAM_ADDR,           1 );
        memcpy( &( configdata->ModulationParams.Params.Rng.CodingRate ),      buffer + MOD_RNG_CODERATE_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Rng.PreambleLength ),      buffer + PAK_RNG_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
        memcpy( &( configdata->PacketParams.Params.Rng.HeaderType ),          buffer + PAK_RNG_HEADERTYPE_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.Rng.PayloadLength ),       buffer + PAK_RNG_PL_LEN_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->PacketParams.Params.Rng.Crc ),                 buffer + PAK_RNG_CRC_MODE_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Rng.InvertIQ ),            buffer + PAK_RNG_IQ_INV_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->RFParamSettings.RngRequestCount ),                 buffer + APP_RNG_REQ_COUNT_RF_PARAM_ADDR,    1 );
        memcpy( &( configdata->RFParamSettings.RngFullScale ),                    buffer + APP_RNG_FULLSCALE_RF_PARAM_ADDR,    2 );
        memcpy( &( configdata->RFParamSettings.RngZscoreMax ),                    buffer + APP_RNG_ZSCORE_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->RFParamSettings.RngAddress ),                      buffer + APP_RNG_ADDR_RF_PARAM_ADDR,         4 );
        memcpy( &( configdata->RFParamSettings.RngAntenna ),                      buffer + APP_RNG_ANT_RF_PARAM_ADDR,          1 );
        memcpy( &( configdata->RFParamSettings.RngUnit ),                         buffer + APP_RNG_UNIT_RF_PARAM_ADDR,         1 );

        //configdata->ModulationParams.PacketType   = RF_PACKET_TYPE_RANGING;
        //configdata->PacketParams.PacketType       = RF_PACKET_TYPE_RANGING;
        configdata->RFParamSettings.ModulationParam1 = configdata->ModulationParams.Params.LoRa.SpreadingFactor;
        configdata->RFParamSettings.ModulationParam2 = configdata->ModulationParams.Params.LoRa.Bandwidth;
        configdata->RFParamSettings.ModulationParam3 = configdata->ModulationParams.Params.LoRa.CodingRate;

        configdata->RFParamSettings.PacketParam1 = configdata->PacketParams.Params.LoRa.PreambleLength;
        configdata->RFParamSettings.PacketParam2 = configdata->PacketParams.Params.LoRa.HeaderType;
        configdata->RFParamSettings.PacketParam3 = configdata->PacketParams.Params.LoRa.PayloadLength;
        configdata->RFParamSettings.PacketParam4 = configdata->PacketParams.Params.LoRa.Crc;
        configdata->RFParamSettings.PacketParam5 = configdata->PacketParams.Params.LoRa.InvertIQ;
        configdata->RFParamSettings.PacketParam6 = 0x00;
        configdata->RFParamSettings.PacketParam7 = 0x00;
    }
    else if( modulation == RF_PACKET_TYPE_FLRC )
    {
        //printf("Load Settings PACKET_TYPE_FLRC\n\r");
        memcpy( &( configdata->ModulationParams.Params.Flrc.BitrateBandwidth ),  buffer + MOD_FLR_BRBW_RF_PARAM_ADDR,         1 );
        memcpy( &( configdata->ModulationParams.Params.Flrc.CodingRate ),        buffer + MOD_FLR_CODERATE_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->ModulationParams.Params.Flrc.ModulationShaping ), buffer + MOD_FLR_MOD_SHAP_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.PreambleLength ),        buffer + PAK_FLR_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.SyncWordLength ),        buffer + PAK_FLR_SYNC_LEN_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.SyncWordMatch ),         buffer + PAK_FLR_SYNC_MATCH_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.HeaderType ),            buffer + PAK_FLR_HEADERTYPE_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.PayloadLength ),         buffer + PAK_FLR_PL_LEN_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.CrcLength ),             buffer + PAK_FLR_CRC_LEN_RF_PARAM_ADDR,      1 );
        memcpy( &( configdata->PacketParams.Params.Flrc.Whitening ),             buffer + PAK_FLR_WHITENING_RF_PARAM_ADDR,    1 );

        //configdata->ModulationParams.PacketType   = RF_PACKET_TYPE_FLRC;
        //configdata->PacketParams.PacketType       = RF_PACKET_TYPE_FLRC;
        configdata->RFParamSettings.ModulationParam1 = configdata->ModulationParams.Params.Flrc.BitrateBandwidth;
        configdata->RFParamSettings.ModulationParam2 = configdata->ModulationParams.Params.Flrc.CodingRate;
        configdata->RFParamSettings.ModulationParam3 = configdata->ModulationParams.Params.Flrc.ModulationShaping;

        configdata->RFParamSettings.PacketParam1 = configdata->PacketParams.Params.Flrc.PreambleLength;
        configdata->RFParamSettings.PacketParam2 = configdata->PacketParams.Params.Flrc.SyncWordLength;
        configdata->RFParamSettings.PacketParam3 = configdata->PacketParams.Params.Flrc.SyncWordMatch;
        configdata->RFParamSettings.PacketParam4 = configdata->PacketParams.Params.Flrc.HeaderType;
        configdata->RFParamSettings.PacketParam5 = configdata->PacketParams.Params.Flrc.PayloadLength;
        configdata->RFParamSettings.PacketParam6 = configdata->PacketParams.Params.Flrc.CrcLength;
        configdata->RFParamSettings.PacketParam7 = configdata->PacketParams.Params.Flrc.Whitening;
    }
    else if( modulation == RF_PACKET_TYPE_GFSK )// GFSK
    {
        //printf("Load Settings PACKET_TYPE_GFSK\n\r");
        memcpy( &( configdata->ModulationParams.Params.Gfsk.BitrateBandwidth ),  buffer + MOD_GFS_BRBW_RF_PARAM_ADDR,         1 );
        memcpy( &( configdata->ModulationParams.Params.Gfsk.ModulationIndex ),   buffer + MOD_GFS_MOD_IND_RF_PARAM_ADDR,      1 );
        memcpy( &( configdata->ModulationParams.Params.Gfsk.ModulationShaping ), buffer + MOD_GFS_MOD_SHAP_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.PreambleLength ),        buffer + PAK_GFS_PREAMBLE_LEN_RF_PARAM_ADDR, 1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.SyncWordLength ),        buffer + PAK_GFS_SYNC_LEN_RF_PARAM_ADDR,     1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.SyncWordMatch ),         buffer + PAK_GFS_SYNC_MATCH_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.HeaderType ),            buffer + PAK_GFS_HEADERTYPE_RF_PARAM_ADDR,   1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.PayloadLength ),         buffer + PAK_GFS_PL_LEN_RF_PARAM_ADDR,       1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.CrcLength ),             buffer + PAK_GFS_CRC_LEN_RF_PARAM_ADDR,      1 );
        memcpy( &( configdata->PacketParams.Params.Gfsk.Whitening ),             buffer + PAK_GFS_WHITENING_RF_PARAM_ADDR,    1 );

        //configdata->ModulationParams.PacketType   = RF_PACKET_TYPE_GFSK;
        //configdata->PacketParams.PacketType       = RF_PACKET_TYPE_GFSK;
        configdata->RFParamSettings.ModulationParam1 = configdata->ModulationParams.Params.Gfsk.BitrateBandwidth;
        configdata->RFParamSettings.ModulationParam2 = configdata->ModulationParams.Params.Gfsk.ModulationIndex;
        configdata->RFParamSettings.ModulationParam3 = configdata->ModulationParams.Params.Gfsk.ModulationShaping;

        configdata->RFParamSettings.PacketParam1 = configdata->PacketParams.Params.Gfsk.PreambleLength;
        configdata->RFParamSettings.PacketParam2 = configdata->PacketParams.Params.Gfsk.SyncWordLength;
        configdata->RFParamSettings.PacketParam3 = configdata->PacketParams.Params.Gfsk.SyncWordMatch;
        configdata->RFParamSettings.PacketParam4 = configdata->PacketParams.Params.Gfsk.HeaderType;
        configdata->RFParamSettings.PacketParam5 = configdata->PacketParams.Params.Gfsk.PayloadLength;
        configdata->RFParamSettings.PacketParam6 = configdata->PacketParams.Params.Gfsk.CrcLength;
        configdata->RFParamSettings.PacketParam7 = configdata->PacketParams.Params.Gfsk.Whitening;
    }

    //configdata->RFParamSettings.ModulationType = modulation;
}


void DataFlashSetDefaultSettings( uint8_t *buffer,ConfigData_t *configdata)
{
    uint32_t g_multicast_id[8][4]={{0xFFFFFFFE},{},{},{},{},{},{},{}};
    static uint32_t s_dev_id;

    s_dev_id = LoRaMAC_IDHashProgram();

    //MSG_DEBUG(0,"Set Default Settings\n\r");


    configdata->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
    //configdata->ModulationParams.PacketType = RF_PACKET_TYPE_RANGING;
    //configdata->PacketParams.PacketType     = RF_PACKET_TYPE_RANGING;//

    configdata->RFParamSettings.ModulationParam1 = RF_LORA_SF8;//
    configdata->RFParamSettings.ModulationParam2 = RF_LORA_BW_1600; //
    configdata->RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

    configdata->RFParamSettings.PacketParam1    = 12; // PreambleLength
    configdata->RFParamSettings.PacketParam2    = RF_LORA_PACKET_VARIABLE_LENGTH;
    configdata->RFParamSettings.PacketParam3    = 255; // PayloadLength
    configdata->RFParamSettings.PacketParam4    = RF_LORA_CRC_ON;
    configdata->RFParamSettings.PacketParam5    = RF_LORA_IQ_NORMAL;
    configdata->RFParamSettings.RngRequestCount = 40;
    configdata->RFParamSettings.RngFullScale    = 30;
    configdata->RFParamSettings.RngZscoreMax    = 1;
    configdata->RFParamSettings.RngAddress      = s_dev_id;//DEMO_RNG_ADDR_1;//DEMO_RNG_ADDR_1;
    configdata->RFParamSettings.RngAntenna      = DEMO_RNG_ANT_1;
    configdata->RFParamSettings.RngUnit         = DEMO_RNG_UNIT_SEL_M;

    DataFlashSaveSettings(buffer,configdata,false, RF_PACKET_TYPE_RANGING);

    configdata->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
    //configdata->ModulationParams.PacketType = RF_PACKET_TYPE_LORA;
    //configdata->PacketParams.PacketType     = RF_PACKET_TYPE_LORA;

    configdata->RFParamSettings.ModulationParam1 = RF_LORA_SF8;
    configdata->RFParamSettings.ModulationParam2 = RF_LORA_BW_1600;
    configdata->RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

    configdata->RFParamSettings.PacketParam1 = 12 ; // PreambleLength
    configdata->RFParamSettings.PacketParam2 = RF_LORA_PACKET_VARIABLE_LENGTH;
    configdata->RFParamSettings.PacketParam3 = DEMO_GFS_LORA_MAX_PAYLOAD;
    configdata->RFParamSettings.PacketParam4 = RF_LORA_CRC_ON;
    configdata->RFParamSettings.PacketParam5 = RF_LORA_IQ_NORMAL;

    DataFlashSaveSettings(buffer,configdata,false, RF_PACKET_TYPE_LORA );

    configdata->RFParamSettings.ModulationType = RF_PACKET_TYPE_GFSK;
    //configdata->ModulationParams.PacketType = RF_PACKET_TYPE_GFSK;
    //configdata->PacketParams.PacketType     = RF_PACKET_TYPE_GFSK;

    configdata->RFParamSettings.ModulationParam1 = RF_GFSK_BLE_BR_0_125_BW_0_3;
    configdata->RFParamSettings.ModulationParam2 = RF_GFSK_BLE_MOD_IND_1_00;
    configdata->RFParamSettings.ModulationParam3 = RF_RADIO_MOD_SHAPING_BT_1_0;

    configdata->RFParamSettings.PacketParam1 = RF_PREAMBLE_LENGTH_32_BITS;
    configdata->RFParamSettings.PacketParam2 = RF_GFSK_SYNCWORD_LENGTH_5_BYTE;
    configdata->RFParamSettings.PacketParam3 = RF_RADIO_RX_MATCH_SYNCWORD_1;
    configdata->RFParamSettings.PacketParam4 = RF_RADIO_PACKET_VARIABLE_LENGTH;
    configdata->RFParamSettings.PacketParam5 = DEMO_GFS_LORA_MAX_PAYLOAD;
    configdata->RFParamSettings.PacketParam6 = RF_RADIO_CRC_2_BYTES;
    configdata->RFParamSettings.PacketParam7 = RF_RADIO_WHITENING_ON;

    DataFlashSaveSettings(buffer,configdata,false, RF_PACKET_TYPE_GFSK );

    configdata->RFParamSettings.ModulationType = RF_PACKET_TYPE_FLRC;
    //configdata->ModulationParams.PacketType = RF_PACKET_TYPE_FLRC;
    //configdata->PacketParams.PacketType     = RF_PACKET_TYPE_FLRC;

    configdata->RFParamSettings.ModulationParam1 = RF_FLRC_BR_0_260_BW_0_3;
    configdata->RFParamSettings.ModulationParam2 = RF_FLRC_CR_1_2;
    configdata->RFParamSettings.ModulationParam3 = RF_RADIO_MOD_SHAPING_BT_1_0;

    configdata->RFParamSettings.PacketParam1 = RF_PREAMBLE_LENGTH_32_BITS;
    configdata->RFParamSettings.PacketParam2 = RF_FLRC_SYNCWORD_LENGTH_4_BYTE;
    configdata->RFParamSettings.PacketParam3 = RF_RADIO_RX_MATCH_SYNCWORD_1;
    configdata->RFParamSettings.PacketParam4 = RF_RADIO_PACKET_VARIABLE_LENGTH;
    configdata->RFParamSettings.PacketParam5 = DEMO_FLRC_MAX_PAYLOAD;
    configdata->RFParamSettings.PacketParam6 = RF_RADIO_CRC_3_BYTES;
    configdata->RFParamSettings.PacketParam7 = RF_RADIO_WHITENING_OFF;


    configdata->RFParamSettings.Entity                = RF_SLAVE;  // RF_MASTER;  //
    configdata->RFParamSettings.AntennaSwitch         = 0x00;
    configdata->RFParamSettings.RadioPowerMode        = RF_USE_DCDC;
    configdata->RFParamSettings.Frequency             = DEMO_CENTRAL_FREQ_PRESET1;
    configdata->RFParamSettings.TxPower               = 13; //DEMO_POWER_TX_MAX;
    configdata->RFParamSettings.MaxNumPacket          = 0x00; // infinite
    configdata->RFParamSettings.ModulationType        = RF_PACKET_TYPE_RANGING;
    configdata->RFParamSettings.ChannelsCount         = 0xFFFFFFFFFF;
    configdata->RFParamSettings.cot_dev_id         	  = s_dev_id;
    configdata->RFParamSettings.cot_multicast_id   	  = g_multicast_id[0][0];
    configdata->RFParamSettings.cot_broadcast_id   	  = 0xFFFFFFFF;

    DataFlashSaveSettings(buffer,configdata,true,RF_PACKET_TYPE_FLRC );

    //DataFlashSaveSettings(buffer,configdata,true, RF_PACKET_TYPE_NONE );

	#if 0
    log_printf("\ndev_id %2x,%2x,%2x,%2x\n",configdata->RFParamSettings.cot_dev_id[0],
                                    configdata->RFParamSettings.cot_dev_id[1],
                                    configdata->RFParamSettings.cot_dev_id[2],
                                    configdata->RFParamSettings.cot_dev_id[3]);
	#endif

}

/*!
 * \brief Erase a page of Flash. Here used to Erase g_rf_param region.
 *
 * \param [in]  page          address of page to erase
 * \param [in]  banks         address of banks to erase
 */
void FlashPageErase( uint32_t page, uint32_t banks )
{
	#if 0
    // Check the parameters
    assert_param( IS_FLASH_PAGE( page ) );
    assert_param( IS_FLASH_BANK_EXCLUSIVE( banks ) );

    if( ( banks & FLASH_BANK_1 ) != RESET )
    {
        CLEAR_BIT( FLASH->CR, FLASH_CR_BKER );
    }
    else
    {
        SET_BIT( FLASH->CR, FLASH_CR_BKER );
    }

    // Proceed to erase the page
    MODIFY_REG( FLASH->CR, FLASH_CR_PNB, ( page << 3 ) );
    SET_BIT( FLASH->CR, FLASH_CR_PER );
    SET_BIT( FLASH->CR, FLASH_CR_STRT );
	#endif

}

/*!
 * \brief Write g_rf_param to emulated rf_param (in fact in Flash " higher address).
 *
 * \param [in]  addr          address of data (g_rf_param offset not to be include)
 * \param [in]  buffer        buffer to use for copy
 * \param [in]  size          size of data to copy
 *
 * \retval      status        Status of operation (SUCCESS, ..)
 */
uint8_t DataFlashMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size )
{

    if( NULL == buffer )
        return 1 ;

    uint8_t *flash = ( uint8_t* )buffer;
    int32_t flash_fd ;

    if( size & 0x03 )/* flash must be writen by  word. */
    {
        size = size-(size & 0x03)+4;
    }

    flash_fd = open( PATH_FLASH, NULL );  /* open file */
    lseek( flash_fd, DATA_RF_PARAM_BASE , SEEK_SET);
    write(flash_fd,flash,size);
    close(flash_fd);

    return 0;

}

uint8_t DataFlashMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size )
{

    assert_param( buffer != NULL );

    assert_param( addr >= DATA_RF_PARAM_BASE );
    assert_param( buffer != NULL );
    assert_param( size < ( DATA_RF_PARAM_END - DATA_RF_PARAM_BASE ) );

    memcpy( buffer, ( uint8_t* )DATA_RF_PARAM_BASE, size );

    return 0;
}

static MemTestStruct_t DataFlashDataCheckSum(uint8_t *buffer)
{
    MemTestStruct_t memTestStruct;
    uint8_t x;
    uint8_t i;
    uint16_t crcBuf;
    memTestStruct.Value = 0xFFFF;

    for( i = 0; i < RF_PARAM_BUFFER_SIZE - sizeof( uint16_t ); i++ )
    {
        x = memTestStruct.Value >> 8 ^ buffer[i];
        x ^= x >> 4;
        memTestStruct.Value = ( memTestStruct.Value << 8 ) ^ \
                              ( ( uint16_t )( x << 12 ) ) ^ \
                              ( ( uint16_t )( x << 5 ) ) ^ \
                              ( ( uint16_t )x );
    }
    memcpy( &crcBuf, buffer + RF_PARAM_CRC_RF_PARAM_ADDR, 2 );
    memTestStruct.Valid = ( crcBuf == memTestStruct.Value );

    return memTestStruct;
}


static uint32_t LoRaMAC_BKDRHash(uint8_t *str)
{
    uint32_t seed = 131;
    uint32_t hash = 0;

    while(*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}


/**
* @Description: 获取设备ID
* @return 设备ID
*/
static uint32_t LoRaMAC_IDHashProgram(void)
{

    am_hal_mcuctrl_device_t psDevice;

	uint8_t NodeID_first[32] ;

    am_hal_mcuctrl_device_info_get(&psDevice);

	sprintf((char*)NodeID_first,"%x,%x",psDevice.ui32ChipID0,psDevice.ui32ChipID1);

	return LoRaMAC_BKDRHash(NodeID_first);


}




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
int32_t sample_rf_param_get(ConfigData_t ConfigData, PHYParamSettings_t *phy_param_setting )
{
    if( !phy_param_setting )
        return ERR_PARAMETER;

    uint8_t ModulationType = ConfigData.RFParamSettings.ModulationType ;
	phy_param_setting->cot_dev_id = ConfigData.RFParamSettings.cot_dev_id;
	phy_param_setting->cot_multicast_id = ConfigData.RFParamSettings.cot_multicast_id;
	phy_param_setting->cot_broadcast_id = ConfigData.RFParamSettings.cot_broadcast_id;

    //memcpy(phy_param_setting->cot_dev_id,ConfigData.RFParamSettings.cot_dev_id,4);
    //memcpy(phy_param_setting->cot_multicast_id,ConfigData.RFParamSettings.cot_multicast_id,4);
    //memcpy(phy_param_setting->cot_broadcast_id,ConfigData.RFParamSettings.cot_broadcast_id,4);
    phy_param_setting->ChannelsCount = ConfigData.RFParamSettings.ChannelsCount;

    phy_param_setting->RF_RadioPacketTypes = (RF_RadioPacketTypes_t)ConfigData.RFParamSettings.ModulationType;

    phy_param_setting->Entity = ConfigData.RFParamSettings.Entity;  //RF_SLAVE;
    phy_param_setting->AntennaSwitch  = ConfigData.RFParamSettings.AntennaSwitch;
    phy_param_setting->Frequency      = ConfigData.RFParamSettings.Frequency;
    phy_param_setting->TxPower        = ConfigData.RFParamSettings.TxPower; //[-18 - 13 dBm]
    phy_param_setting->RF_RadioRegulatorModes = (RF_RadioRegulatorModes_t)ConfigData.RFParamSettings.RadioPowerMode;


    if (ModulationType == RF_PACKET_TYPE_LORA)
    {
        // LORA ============
        phy_param_setting->PHYModulationParams.Params.LoRa.SpreadingFactor = ( RF_RadioLoRaSpreadingFactors_t )ConfigData.RFParamSettings.ModulationParam1 ;
        phy_param_setting->PHYModulationParams.Params.LoRa.Bandwidth = (RF_RadioLoRaBandwidths_t)ConfigData.RFParamSettings.ModulationParam2;
        phy_param_setting->PHYModulationParams.Params.LoRa.CodingRate = ( RF_RadioLoRaCodingRates_t )ConfigData.RFParamSettings.ModulationParam3;

        phy_param_setting->PHYPacketParams.Params.LoRa.PreambleLength = ConfigData.RFParamSettings.PacketParam1;// PreambleLength
        phy_param_setting->PHYPacketParams.Params.LoRa.HeaderType = (RF_RadioLoRaPacketLengthsModes_t)ConfigData.RFParamSettings.PacketParam2;
        phy_param_setting->PHYPacketParams.Params.LoRa.PayloadLength = ConfigData.RFParamSettings.PacketParam3; //
        phy_param_setting->PHYPacketParams.Params.LoRa.Crc = (RF_RadioLoRaCrcModes_t)ConfigData.RFParamSettings.PacketParam4;
        phy_param_setting->PHYPacketParams.Params.LoRa.InvertIQ  = ( RF_RadioLoRaIQModes_t )ConfigData.RFParamSettings.PacketParam5;

    }
    else if(ModulationType == RF_PACKET_TYPE_GFSK)
    {
        //GFSK ============
        phy_param_setting->PHYModulationParams.Params.Gfsk.BitrateBandwidth = ( RF_RadioGfskBleBitrates_t )ConfigData.RFParamSettings.ModulationParam1;
        phy_param_setting->PHYModulationParams.Params.Gfsk.ModulationIndex = (RF_RadioGfskBleModIndexes_t)ConfigData.RFParamSettings.ModulationParam2;
        phy_param_setting->PHYModulationParams.Params.Gfsk.ModulationShaping = (RF_RadioModShapings_t)ConfigData.RFParamSettings.ModulationParam2;

        phy_param_setting->PHYPacketParams.Params.Gfsk.PreambleLength = ( RF_RadioPreambleLengths_t )ConfigData.RFParamSettings.PacketParam1;    //!< The preamble length for GFSK packet type
        phy_param_setting->PHYPacketParams.Params.Gfsk.SyncWordLength =  ( RF_RadioSyncWordLengths_t )ConfigData.RFParamSettings.PacketParam2;    //!< The synchronization word length for GFSK packet type
        phy_param_setting->PHYPacketParams.Params.Gfsk.SyncWordMatch = ( RF_RadioSyncWordRxMatchs_t )ConfigData.RFParamSettings.PacketParam3;     //!< The synchronization correlator to use to check synchronization word
        phy_param_setting->PHYPacketParams.Params.Gfsk.HeaderType = (RF_RadioPacketLengthModes_t)ConfigData.RFParamSettings.PacketParam4;       //!< If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted
        phy_param_setting->PHYPacketParams.Params.Gfsk.PayloadLength = ConfigData.RFParamSettings.PacketParam5;     //!< Size of the payload in the GFSK packet
        phy_param_setting->PHYPacketParams.Params.Gfsk.CrcLength = ( RF_RadioCrcTypes_t )ConfigData.RFParamSettings.PacketParam6;         //!< Size of the CRC block in the GFSK packet
        phy_param_setting->PHYPacketParams.Params.Gfsk.Whitening = ( RF_RadioWhiteningModes_t )ConfigData.RFParamSettings.PacketParam7;//!< Usage of whitening on payload and CRC blocks plus header block if header type is variable
    }
    else if(ModulationType == RF_PACKET_TYPE_FLRC)
    {
        // FLRC  ==================
        phy_param_setting->PHYModulationParams.Params.Flrc.BitrateBandwidth = ( RF_RadioFlrcBitrates_t )ConfigData.RFParamSettings.ModulationParam1;  //!< The bandwidth and bit-rate values for FLRC modulation
        phy_param_setting->PHYModulationParams.Params.Flrc.CodingRate = ( RF_RadioFlrcCodingRates_t )ConfigData.RFParamSettings.ModulationParam2;       //!< The coding rate for FLRC modulation
        phy_param_setting->PHYModulationParams.Params.Flrc.ModulationShaping = ( RF_RadioModShapings_t )ConfigData.RFParamSettings.ModulationParam2; //!< The modulation shaping for FLRC modulation

        phy_param_setting->PHYPacketParams.Params.Flrc.PreambleLength = ( RF_RadioPreambleLengths_t ) ConfigData.RFParamSettings.PacketParam1;    //!< The preamble length for FLRC packet type
        phy_param_setting->PHYPacketParams.Params.Flrc.SyncWordLength = (RF_RadioFlrcSyncWordLengths_t)ConfigData.RFParamSettings.PacketParam2;    //!< The synchronization word length for FLRC packet type
        phy_param_setting->PHYPacketParams.Params.Flrc.SyncWordMatch = (RF_RadioSyncWordRxMatchs_t)ConfigData.RFParamSettings.PacketParam3;     //!< The synchronization correlator to use to check synchronization word
        phy_param_setting->PHYPacketParams.Params.Flrc.HeaderType = (RF_RadioPacketLengthModes_t)ConfigData.RFParamSettings.PacketParam4;        //!< If the header is explicit, it will be transmitted in the FLRC packet. If the header is implicit, it will not be transmitted.
        phy_param_setting->PHYPacketParams.Params.Flrc.PayloadLength = ConfigData.RFParamSettings.PacketParam5;     //!< Size of the payload in the FLRC packet
        phy_param_setting->PHYPacketParams.Params.Flrc.CrcLength = (RF_RadioCrcTypes_t)ConfigData.RFParamSettings.PacketParam6;        //!< Size of the CRC block in the FLRC packet
        phy_param_setting->PHYPacketParams.Params.Flrc.Whitening = (RF_RadioWhiteningModes_t)ConfigData.RFParamSettings.PacketParam7;        //!< Usage of whitening on payload and CRC blocks plus header block if header type is variable

    }
    else if(ModulationType == RF_PACKET_TYPE_BLE)
    {
        // BLE  =======================
        phy_param_setting->PHYModulationParams.Params.Ble.BitrateBandwidth = (RF_RadioGfskBleBitrates_t)ConfigData.RFParamSettings.ModulationParam1;  //!< The bandwidth and bit-rate values for BLE and GFSK modulations
        phy_param_setting->PHYModulationParams.Params.Ble.ModulationIndex = (RF_RadioGfskBleModIndexes_t)ConfigData.RFParamSettings.ModulationParam2;   //!< The coding rate for BLE and GFSK modulations
        phy_param_setting->PHYModulationParams.Params.Ble.ModulationShaping = (RF_RadioModShapings_t)ConfigData.RFParamSettings.ModulationParam3; //!< The modulation shaping for BLE and GFSK modulations

        phy_param_setting->PHYPacketParams.Params.Ble.ConnectionState = (RF_RadioBleConnectionStates_t)ConfigData.RFParamSettings.PacketParam1;   //!< The BLE state
        phy_param_setting->PHYPacketParams.Params.Ble.CrcLength = (RF_RadioBleCrcTypes_t)ConfigData.RFParamSettings.PacketParam2;          //!< Size of the CRC block in the BLE packet
        phy_param_setting->PHYPacketParams.Params.Ble.BleTestPayload = (RF_RadioBleTestPayloads_t)ConfigData.RFParamSettings.PacketParam3;     //!< Special BLE packet types
        phy_param_setting->PHYPacketParams.Params.Ble.Whitening = (RF_RadioWhiteningModes_t)ConfigData.RFParamSettings.PacketParam4;         //!< Usage of whitening on PDU and CRC blocks of BLE packet
    }
    else
    {
       // RNG ============
        phy_param_setting->RF_RadioPacketTypes = RF_PACKET_TYPE_RANGING;
        phy_param_setting->PHYModulationParams.Params.Rng.RngRequestCount = ConfigData.RFParamSettings.RngRequestCount; //40;
        phy_param_setting->PHYModulationParams.Params.Rng.RngFullScale    = ConfigData.RFParamSettings.RngFullScale;
        phy_param_setting->PHYModulationParams.Params.Rng.RngZscoreMax    = ConfigData.RFParamSettings.RngZscoreMax;
        phy_param_setting->PHYModulationParams.Params.Rng.RngAddress      = ConfigData.RFParamSettings.RngAddress;// DEMO_RNG_ADDR_1;
        phy_param_setting->PHYModulationParams.Params.Rng.RngAntenna      = ConfigData.RFParamSettings.RngAntenna;//DEMO_RNG_ANT_1;
        phy_param_setting->PHYModulationParams.Params.Rng.RngUnit         = ConfigData.RFParamSettings.RngUnit;//DEMO_RNG_UNIT_SEL_M;

        phy_param_setting->PHYModulationParams.Params.LoRa.SpreadingFactor = ( RF_RadioLoRaSpreadingFactors_t )ConfigData.RFParamSettings.ModulationParam1 ;
        phy_param_setting->PHYModulationParams.Params.LoRa.Bandwidth = (RF_RadioLoRaBandwidths_t)ConfigData.RFParamSettings.ModulationParam2;
        phy_param_setting->PHYModulationParams.Params.LoRa.CodingRate = ( RF_RadioLoRaCodingRates_t )ConfigData.RFParamSettings.ModulationParam3;

        phy_param_setting->PHYPacketParams.Params.LoRa.PreambleLength = ConfigData.RFParamSettings.PacketParam1;// PreambleLength
        phy_param_setting->PHYPacketParams.Params.LoRa.HeaderType = (RF_RadioLoRaPacketLengthsModes_t)ConfigData.RFParamSettings.PacketParam2;
        phy_param_setting->PHYPacketParams.Params.LoRa.PayloadLength = ConfigData.RFParamSettings.PacketParam3; //
        phy_param_setting->PHYPacketParams.Params.LoRa.Crc = (RF_RadioLoRaCrcModes_t)ConfigData.RFParamSettings.PacketParam4;
        phy_param_setting->PHYPacketParams.Params.LoRa.InvertIQ  = ( RF_RadioLoRaIQModes_t )ConfigData.RFParamSettings.PacketParam5;
    }

    return ERR_SUCCESS;
}



/**
* @name: get_rf_param
* This funtion is to get rf params
* @param[in]   g_rf_param -- the point of rf params.
* @param[out]  g_rf_param -- the point of rf params
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-07-10
*/
int32_t get_rf_param( ConfigData_t *rf_config_param )
{
    if( !rf_config_param )
        return ERR_PARAMETER;
    switch( rf_config_param->RFParamSettings.ModulationType )
    {
        case RF_PACKET_TYPE_LORA:
            rf_config_param->RFParamSettings.ModulationParam1 = (uint8_t)rf_config_param->ModulationParams.Params.LoRa.SpreadingFactor;
            rf_config_param->RFParamSettings.ModulationParam2 = (uint8_t)rf_config_param->ModulationParams.Params.LoRa.Bandwidth;
            rf_config_param->RFParamSettings.ModulationParam3 = (uint8_t)rf_config_param->ModulationParams.Params.LoRa.CodingRate;
            rf_config_param->RFParamSettings.PacketParam1 =     (uint8_t)rf_config_param->PacketParams.Params.LoRa.PreambleLength;
            rf_config_param->RFParamSettings.PacketParam2 =     (uint8_t)rf_config_param->PacketParams.Params.LoRa.HeaderType;
            rf_config_param->RFParamSettings.PacketParam3 =     (uint8_t)rf_config_param->PacketParams.Params.LoRa.PayloadLength;
            rf_config_param->RFParamSettings.PacketParam4 =     (uint8_t)rf_config_param->PacketParams.Params.LoRa.Crc;
            rf_config_param->RFParamSettings.PacketParam5 =     (uint8_t)rf_config_param->PacketParams.Params.LoRa.InvertIQ;
            rf_config_param->RFParamSettings.PacketParam6 =     0x0;
            rf_config_param->RFParamSettings.PacketParam7 =     0x0;
            break;

        case RF_PACKET_TYPE_RANGING:

            rf_config_param->RFParamSettings.ModulationParam1 = (uint8_t)rf_config_param->ModulationParams.Params.Rng.SpreadingFactor;
            rf_config_param->RFParamSettings.ModulationParam2 = (uint8_t)rf_config_param->ModulationParams.Params.Rng.Bandwidth;
            rf_config_param->RFParamSettings.ModulationParam3 = (uint8_t)rf_config_param->ModulationParams.Params.Rng.CodingRate;
            rf_config_param->RFParamSettings.PacketParam1 =     (uint8_t)rf_config_param->PacketParams.Params.Rng.PreambleLength;
            rf_config_param->RFParamSettings.PacketParam2 =     (uint8_t)rf_config_param->PacketParams.Params.Rng.HeaderType;
            rf_config_param->RFParamSettings.PacketParam3 =     (uint8_t)rf_config_param->PacketParams.Params.Rng.PayloadLength;
            rf_config_param->RFParamSettings.PacketParam4 =     (uint8_t)rf_config_param->PacketParams.Params.Rng.Crc;
            rf_config_param->RFParamSettings.PacketParam5 =     (uint8_t)rf_config_param->PacketParams.Params.Rng.InvertIQ;
            rf_config_param->RFParamSettings.PacketParam6 =     0x0;
            rf_config_param->RFParamSettings.PacketParam7 =     0x0;

            rf_config_param->RFParamSettings.RngRequestCount =  rf_config_param->ModulationParams.Params.Rng.RngRequestCount;
            rf_config_param->RFParamSettings.RngFullScale =     rf_config_param->ModulationParams.Params.Rng.RngFullScale;
            rf_config_param->RFParamSettings.RngZscoreMax =     rf_config_param->ModulationParams.Params.Rng.RngZscoreMax;
            rf_config_param->RFParamSettings.RngAddress =       rf_config_param->ModulationParams.Params.Rng.RngAddress;
            rf_config_param->RFParamSettings.RngAntenna =       rf_config_param->ModulationParams.Params.Rng.RngAntenna;
            rf_config_param->RFParamSettings.RngUnit =          rf_config_param->ModulationParams.Params.Rng.RngUnit;
            break;

        case RF_PACKET_TYPE_FLRC:
            rf_config_param->RFParamSettings.ModulationParam1 = (uint8_t)rf_config_param->ModulationParams.Params.Flrc.BitrateBandwidth;
            rf_config_param->RFParamSettings.ModulationParam2 = (uint8_t)rf_config_param->ModulationParams.Params.Flrc.CodingRate;
            rf_config_param->RFParamSettings.ModulationParam3 = (uint8_t)rf_config_param->ModulationParams.Params.Flrc.ModulationShaping;
            rf_config_param->RFParamSettings.PacketParam1 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.PreambleLength;
            rf_config_param->RFParamSettings.PacketParam2 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.SyncWordLength;
            rf_config_param->RFParamSettings.PacketParam3 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.SyncWordMatch;
            rf_config_param->RFParamSettings.PacketParam4 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.HeaderType;
            rf_config_param->RFParamSettings.PacketParam5 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.PayloadLength;
            rf_config_param->RFParamSettings.PacketParam6 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.CrcLength;
            rf_config_param->RFParamSettings.PacketParam7 =     (uint8_t)rf_config_param->PacketParams.Params.Flrc.Whitening;
            break;

        case RF_PACKET_TYPE_GFSK:
            rf_config_param->RFParamSettings.ModulationParam1 = (uint8_t)rf_config_param->ModulationParams.Params.Gfsk.BitrateBandwidth;
            rf_config_param->RFParamSettings.ModulationParam2 = (uint8_t)rf_config_param->ModulationParams.Params.Gfsk.ModulationIndex;
            rf_config_param->RFParamSettings.ModulationParam3 = (uint8_t)rf_config_param->ModulationParams.Params.Gfsk.ModulationShaping;
            rf_config_param->RFParamSettings.PacketParam1 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.PreambleLength;
            rf_config_param->RFParamSettings.PacketParam2 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.SyncWordLength;
            rf_config_param->RFParamSettings.PacketParam3 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.SyncWordMatch;
            rf_config_param->RFParamSettings.PacketParam4 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.HeaderType;
            rf_config_param->RFParamSettings.PacketParam5 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.PayloadLength;
            rf_config_param->RFParamSettings.PacketParam6 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.CrcLength;
            rf_config_param->RFParamSettings.PacketParam7 =     (uint8_t)rf_config_param->PacketParams.Params.Gfsk.Whitening;
            break;

     default:
            return ERR_ERROR;
    }
	return ERR_SUCCESS;
}


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
int32_t set_rf_param( ConfigData_t *rf_config_param,bool set_to_flah_flag)
{
	if( !rf_config_param )
		return ERR_PARAMETER;

    if( true == set_to_flah_flag )
    {
        uint8_t buffer[RF_PARAM_BUFFER_SIZE] = {0};
        DataFlashMcuReadBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );
        DataFlashSaveSettings(buffer,rf_config_param,true,(RF_RadioPacketTypes_t)rf_config_param->RFParamSettings.ModulationType);
    }
    else
    {
        DataFlashSaveSettings(NULL,rf_config_param,false,(RF_RadioPacketTypes_t)rf_config_param->RFParamSettings.ModulationType);
    }

	return ERR_SUCCESS;
}

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
uint32_t get_dev_id(void)
{
	return LoRaMAC_IDHashProgram();
}
