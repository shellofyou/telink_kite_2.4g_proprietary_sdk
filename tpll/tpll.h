/********************************************************************************************************
 * @file     tpll.h
 *
 * @brief    This file provides set of functions for the Telink TPLL Link Layer Controller.
 *
 * @author   jian.zhang@telink-semi.com
 * @date     Jan. 5, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *           The information contained herein is confidential property of Telink 
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *           Co., Ltd. and the licensee or the terms described here-in. This heading 
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this 
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#ifndef _TPLL_H_
#define _TPLL_H_

#define RX_SETTLE_TIME_US        120
#define TX_SETTLE_TIME_US        110
#define PTX_RETRY_DELAY_TIME_US         10

#define         RF_PACKET_LENGTH_OK(p)           (p[0] == (p[12]&0x3f)+15)
#define         RF_PACKET_CRC_OK(p)              ((p[p[0]+3] & 0x51) == 0x40)



/** An enum describing the radio's address width.
 *
 */
typedef enum {
    ADDRESS_WIDTH_3BYTES = 3,      /**< Set address width to 3 bytes */
    ADDRESS_WIDTH_4BYTES,          /**< Set address width to 4 bytes */
    ADDRESS_WIDTH_5BYTES           /**< Set address width to 5 bytes */
} TPLL_AddressWidthTypeDef;

/** An enum describing the pipe IDs.
 *
 */
typedef enum {
    TPLL_PIPE0 = 0,          /**< Select pipe0 */
    TPLL_PIPE1,              /**< Select pipe1 */
    TPLL_PIPE2,              /**< Select pipe2 */
    TPLL_PIPE3,              /**< Select pipe3 */
    TPLL_PIPE4,              /**< Select pipe4 */
    TPLL_PIPE5,              /**< Select pipe5 */
    TPLL_TX,                 /**< Refer to TX address*/
    TPLL_PIPE_ALL = 0xFF     /**< Close or open all pipes*/
} TPLL_PipeIDTypeDef;

/** An enum describing the TPLL mode state-machine status.
 *
 */
typedef enum {
    TPLL_STATE_MACHINE_STATUS_IDLE = 0,          /**< idle */
    TPLL_STATE_MACHINE_STATUS_TX_SETTLE,         /**< tx settle*/
    TPLL_STATE_MACHINE_STATUS_TX,                /**< tx */
    TPLL_STATE_MACHINE_STATUS_RX_WAIT,           /**< rx wait */
    TPLL_STATE_MACHINE_STATUS_RX,                /**< rx */
    TPLL_STATE_MACHINE_STATUS_TX_WAIT,           /**< tx wait */
} TPLL_StatemachineStatusTypeDef;

/** An enum describing the mode of TPLL.
 *
 */
typedef enum {
    TPLL_MODE_PTX = 0,      /**< PTX Mode */
    TPLL_MODE_PRX,          /**< PRX Mode */
} TPLL_ModeTypeDef;

/** An enum describing the datarate.
 *
 */
typedef enum {
    TPLL_DR_1M = 0,      /**< 1MBit/s */
    TPLL_DR_2M,          /**< 2MBit/s */
    TPLL_DR_500k,      /**< 1MBit/s */
    TPLL_DR_250k,          /**< 2MBit/s */

} TPLL_DaterateTypeDef;

/** An enum describing the Modulation Index.
 *
 */
typedef enum {
	TPLL_RF_MI_032 = 0,		  /**< MI = 0.32 */
	TPLL_RF_MI_050 = 1,		  /**< MI = 0.5 */

}TPLL_MIVauleTypeDef;

/** An enum describing the radio output power.
 *
 */

typedef enum {

    TPLL_RF_POWER_10DBM = 58, // 10dbm
    TPLL_RF_POWER_9DBM  = 49, // 9dbm
    TPLL_RF_POWER_8DBM  = 43, // 8dbm
    TPLL_RF_POWER_7DBM  = 37, // 7dbm
    TPLL_RF_POWER_6DBM  = 33, // 6dbm
    TPLL_RF_POWER_5DBM  = 29, // 5dbm
    TPLL_RF_POWER_4DBM  = 25, // 4dbm
    TPLL_RF_POWER_3DBM  = 191,// 3dbm
    TPLL_RF_POWER_2DBM  = 182,// 2dbm
    TPLL_RF_POWER_1DBM  = 174,// 1dbm
    TPLL_RF_POWER_0DBM  = 169,// 0dbm
    TPLL_RF_POWER_M_1DBM  = 164,// -1dbm
    TPLL_RF_POWER_M_2DBM  = 160,// -2dbm
    TPLL_RF_POWER_M_3DBM  = 156,// -3dbm
    TPLL_RF_POWER_M_4DBM  = 152,// -4dbm
    TPLL_RF_POWER_M_5DBM  = 150,// -5dbm
    TPLL_RF_POWER_M_6DBM  = 148,// -6dbm
    TPLL_RF_POWER_M_7DBM  = 146,// -7dbm
    TPLL_RF_POWER_M_8DBM  = 144,// -8dbm
    TPLL_RF_POWER_M_9DBM  = 142,// -9dbm
    TPLL_RF_POWER_M_10DBM  = 140,// -10dbm
    TPLL_RF_POWER_M_11DBM  = 138,// -11dbm
    TPLL_RF_POWER_M_13DBM  = 136,// -13dbm
    TPLL_RF_POWER_M_16DBM  = 134,// -16dbm
    TPLL_RF_POWER_M_20DBM  = 132,// -20dbm
    TPLL_RF_POWER_M_25DBM  = 130,// -25dbm
    TPLL_RF_POWER_M_30DBM  = 0xff,// -30dbm
    TPLL_RF_POWER_M_50dBm  = 128,// -50dbm
} TPLL_OutputPowerTypeDef;



/** Set radio's on-air datarate.
 * Use this function to select radio's on-air
 * datarate.
 *
 * @param datarate On-air datarate
*/
extern void TPLL_SetDatarate(TPLL_DaterateTypeDef datarate);

/** Set radio's RF channel.
 * Use this function to select which RF channel to use.
 *
 * @param channel RF channel
*/
extern void TPLL_SetRFChannel(signed short channel);

/** Set new radio's RF channel.
 * Use this function to select which RF channel to use.
 *
 * @param channel RF channel
*/
extern void TPLL_SetNewRFChannel(signed short channel);

/** Set radio's TX output power.
 * Use this function set the radio's TX output power.
 *
 * @param power Radio's TX output power
*/
extern void TPLL_SetOutputPower(TPLL_OutputPowerTypeDef power);

/** Set TX pipe manually.
 * Use this function to select one pipe as TX pipe.
 *
 * @param pipe_id The pipe is selected as TX pipe
*/
extern void TPLL_SetTXPipe(TPLL_PipeIDTypeDef pipe_id);

/** Get currently manually selected TX pipe .
 * Use this function to get currently selected TX pipe.
 *
 * @return pipe_id The pipe is selected as TX pipe
*/
extern unsigned char TPLL_GetTXPipe(void);

/** Update the pipe's tx-fifo rptr manually.
 * Use this function to update the tx-fifo's rptr.
 *
 * @param pipe_id The pipe is selected as TX pipe.
*/
extern void TPLL_UpdateTXFifoRptr(TPLL_PipeIDTypeDef pipe_id);

/** Enables the dynamic payload length
 * @param enable Whether enable or disable dynamic payload length
 */
extern void TPLL_EnableDynamicPayload(unsigned char enable);

/** Enables the ACK payload feature
 * @param enable Whether to enable or disable ACK payload
 */
extern void TPLL_EnableAckPayload(unsigned char enable);

/** Enables the W_TX_PAYLOAD_NOACK command
 * @param enable Whether to enable or disable NoAck option in 9-bit Packet control field
 */
extern void TPLL_EnableNoAck(unsigned char enable);

/** Function for enabling dynmic payload size.
 * The input parameter contains is a byte where the bit values tells weather the
 * pipe should use dynamic payload size. For example if bit 0 is set then
 * pipe 0 will accept dynamic payload size.
 * @param setup Byte value telling for which pips(s) to enable dynamic payload size
 */
extern void TPLL_SetupDynamicPayload(unsigned char msk);

/** Write payload bytes of the ACK packet
 * Writes the payload that will be transmitted with the ack on the given pipe.
 * @param pipe_id Pipe that transmits the payload
 * @param payload Pointer to the payload data
 * @param length Size of the data to transmit
 */
extern void TPLL_WriteAckPayload(TPLL_PipeIDTypeDef pipe_id, const unsigned char *payload, unsigned char length);

/** Open radio pipe(s) and enable/disable auto acknowledge.
 * Use this function to open one or all pipes,
 * with or without auto acknowledge.
 *
 * @param pipe_id Radio pipe to open
 * @param auto_ack_en Auto_Ack ON/OFF
 * @
*/
extern void TPLL_OpenPipe(TPLL_PipeIDTypeDef pipe_id, unsigned char auto_ack_en);

/** Close radio pipe(s).
 * Use this function to close one pipe or all pipes.
 *
 * @param pipe_id Pipe# number to close
*/
extern void TPLL_ClosePipe(TPLL_PipeIDTypeDef pipe_id);

/** Set radio's RX address and TX address.
 * Use this function to set a RX address, or to set the TX address.
 * Beware of the difference for single and multibyte address registers.
 *
 * @param pipe_id Which pipe to set
 * @param *addr Buffer from which the address is stored in
*/
extern void TPLL_SetAddress(TPLL_PipeIDTypeDef pipe_id, const unsigned char *addr);

/** Get address for selected pipe.
 * Use this function to get address for selected pipe.
 *
 * @param pipe_id Which pipe to get, Pipe- or TX-address
 * @param *addr buffer in which address bytes are written.
 * <BR><BR>For pipes containing only LSB byte of address, this byte is returned
 * in the<BR> *addr buffer.
 *
 * @return Numbers of bytes copied to addr
*/
extern unsigned char TPLL_GetAddress(TPLL_PipeIDTypeDef pipe_id, unsigned char *addr);

/** Set auto acknowledge parameters.
 * Use this function to set retransmit and retransmit delay
 * parameters.
 *
 * @param retr Number of retransmit, 0 equ retransmit OFF
 * @param delay Retransmit delay
*/
extern void TPLL_SetAutoRetry(unsigned char retry_times, unsigned short retry_delay);

/** Set radio's address width.
 * Use this function to define the radio's address width,
 * referes to both RX and TX.
 *
 * @param address_width Address with in bytes
*/
extern void TPLL_SetAddressWidth(TPLL_AddressWidthTypeDef address_width);

/** Gets the radio's address width.
 *
 * @return Address width
 */
extern unsigned char TPLL_GetAddressWidth(void);

/** Set payload width for selected pipe(corresponds to static payload length mode of TPLL)
 * Use this function to set the number of bytes expected
 * on a selected pipe.
 *
 * @param pipe_id Pipe number to set payload width for
 * @param pload_width number of bytes expected
*/
extern void TPLL_SetRxPayloadWidth(TPLL_PipeIDTypeDef pipe_id, unsigned char pload_width);

/** Get pipe status.
 * Use this function to check status for a selected pipe.
 *
 * @param  pipe_id Pipe number to check status for
 *
 * @return Pipe_Status
 * @retval 0x00 Pipe is closed, autoack disabled
 * @retval 0x01 Pipe is open, autoack disabled
 * @retval 0x03 Pipe is open, autoack enabled
*/
extern unsigned char TPLL_GetPipeStatus(TPLL_PipeIDTypeDef pipe_id);

/** Get auto retransmit parameters.
 * Use this function to get the auto retransmit parameters,
 * retrans count and retrans delay.
 *
 * @return AutoRetrans Parameters
 *
 * @retval Upper 4-bit Retransmit Delay
 * @retval Lower 4-bit Retransmit Count
*/
extern unsigned char TPLL_GetAutoRetrStatus(void);

/** Get packet lost counter
 * Use this function to get the packet(s) counter.
 *
 * @return packet lost counter
*/
extern unsigned char TPLL_GetPacketLostCtr(void);

/** Get RX payload width for selected pipe.
 * Use this function to get the expected payload
 * width for selected ppe number.
 *
 * @param pipe_id Pipe number to get payload width for
 *
 * @return Payload_Width in bytes
*/
extern unsigned char TPLL_GetRxPayloadWidth(TPLL_PipeIDTypeDef pipe_id);
//@}

/* Status functions prototypes */

/** Check for TX FIFO empty.
 * Use this function to check if TX FIFO
 * is empty.
 *
 * @return TX FIFO empty bit
 * @retval FALSE TX FIFO NOT empty
 * @retval TRUE TX FIFO empty
 *
*/
extern unsigned char TPLL_TxFifoEmpty(TPLL_PipeIDTypeDef pipe_id);

/** Check for TX FIFO full.
 * Use this function to check if TX FIFO
 * is full.
 *
 * @return TX FIFO full bit
 * @retval FALSE TX FIFO NOT full
 * @retval TRUE TX FIFO full
 *
*/
extern unsigned char TPLL_TxFifoFull(TPLL_PipeIDTypeDef pipe_id);

/** Get radio's TX FIFO status.
 * Use this function to get the radio's TX
 * FIFO status.
 *
 * @param pipe_id specify the pipe
 * @return TX FIFO status
 * @retval 0x00 TX FIFO NOT empty, but NOT full
 * @retval 0x01 FIFO empty
 * @retval 0x02 FIFO full
 *
*/
extern unsigned char TPLL_GetTxFifoStatus(TPLL_PipeIDTypeDef pipe_id);

/** Get radio's transmit attempts status.
 * Use this function to get number of retransmit
 * attempts and number of packet lost.
 *
 * @return Retransmit attempts counters
*/
extern unsigned char TPLL_GetTransmitAttempts(void);

/** Get the carrier detect flag.
 * Use this function to get the carrier detect flag,
 * used to detect stationary disturbance on selected
 * RF channel.
 *
 * @return Carrier Detect
 * @retval FALSE Carrier NOT Detected
 * @retval TRUE Carrier Detected
*/
extern unsigned char TPLL_GetCarrierDetect(void);

/* Data operation prototypes */

/** Get RX data source.
 * Use this function to read which RX pipe data
 * was received on for current top level FIFO data packet.
 *
 * @return pipe number of current packet present
*/
extern unsigned char TPLL_GetRxDataSource(void);

/** Read RX payload.
 * Use this function to read top level payload
 * available in the RX FIFO.
 *
 * @param  *rx_pload pointer to buffer in which RX payload are stored
 * @return  2 bit pid number (MSB byte) and 6 bit packet length (LSB byte)
*/
extern unsigned short TPLL_ReadRxPayload(unsigned char *rx_pload);


/** Get RX timestamp.
 * Use this function to get RX timestamp, we should use TPLL_ReadRxPayload API to update timestamp before get timestamp
 *
 *
 * @param	None
 * @return timestamp
*/

unsigned int TPLL_GetTimestamp(void);

/** Get RX RSSI.
 *  Use this function to get RX RSSI, we should use TPLL_ReadRxPayload API to update RSSI before get RSSI
 *
 *
 * @return timestamp value
*/
signed int TPLL_GetRxRssiValue(void);


/** Write TX payload to radio.
 * Use this function to write a packet of
 * TX payload into the radio.
 * <I>length</I> number of bytes, which are stored in <I>*tx_pload</I>.
 *
 * @param pipe_id pipe number
 * @param *tx_pload pointer to buffer in which TX payload are present
 * @param length number of bytes to write
 * @return the length of payload written in to tx-fifo
 * @retval 0 error
 * @retval >0 success, the length of written bytes
*/
extern unsigned char TPLL_WriteTxPayload(TPLL_PipeIDTypeDef pipe_id, const unsigned char *tx_pload, unsigned char length);

/** Write TX payload which do not require ACK. When transmitting
 * a ACK is not required nor sent from the receiver. The payload will
 * always be assumed as "sent".
 *
 * Use this function to write a packet of
 * TX payload into the radio.
 * <I>length</I> number of bytes, which are stored in <I>*tx_pload</I>.
 *
 * @param *tx_pload pointer to buffer in which TX payload are present
 * @param length number of bytes to write
*/
extern void TPLL_WriteTxPayloadNoAck(const unsigned char *tx_pload, unsigned char length);

/** Reuse TX payload.
 * Use this function to set that the radio is using
 * the last transmitted payload for the next packet as well.
 *
*/
extern void TPLL_ReuseTx(TPLL_PipeIDTypeDef pipe_id);

/** Get status of reuse TX function.
 * Use this function to check if reuse TX payload is
 * activated
 *
 * @return Reuse TX payload mode
 * @retval FALSE Not activated
 * @retval TRUE Activated
*/
extern unsigned char TPLL_GetReuseTxStatus(void);

/** Flush RX FIFO.
 * Use this function to flush the radio's
 * RX FIFO.
 *
*/
extern void TPLL_FlushRx(void);

/** Flush TX FIFO.
 * Use this function to flush the radio's
 * TX FIFO.
 *
*/
extern void TPLL_FlushTx(TPLL_PipeIDTypeDef pipe_id);

/** Trigger the rf transmission of PTX.
 * Use this function to trigger the radio's transmission activity in specified pipe
 * 
 * @param pipe_id specifies the pipe in which the radio transmission will be triggered
 *
*/
extern void TPLL_PTXTrig(void);
extern void TPLL_PRXTrig(void);

/** Set the wait duration from the end of transmitting to the start of receiving(to receive Ack).
 * Use this function to set the wait duration between the end of an Ack-required packet's transmission
 * and the start of Ack receiving to accommodate with another chip.
 * 
 * @param wait_us specifies the wait time between the end of an Ack-required packet's transmission
 *                and the start of Ack receiving
*/
extern void TPLL_RxWaitSet(unsigned short wait_us);
extern void TPLL_TxWaitSet(unsigned short wait_us);

/** Set the rx duration when an Ack-required packet has been transmitted and an Ack is expected.
 * Use this function to set the rx duration 
 *
 * 
 * @param period_us specifies the rx duration 
 * 
*/
extern void TPLL_RxTimeoutSet(unsigned short period_us);
extern void TPLL_TxSettleSet(unsigned short period_us);
extern void TPLL_RxSettleSet(unsigned short period_us);

extern void TPLL_ModeSet(TPLL_ModeTypeDef mode);

extern void TPLL_SetTimerPeriod(unsigned int period_us);
extern unsigned char TPLL_RandGen(unsigned char seed, unsigned char max_limit);

/** Determine whether the received packet is valid, i.e. CRC is correct and length is reasonable.
 * Use this function to determine the valid received packet.
 * 
 *
 * @return 1: if the packet is valid; 0: if the packet is invalid
*/
extern unsigned char TPLL_IsRxPacketValid(void);

extern void TPLL_SetTxMI(unsigned short mi_value);
extern void  TPLL_SetRxMI(unsigned short mi_value);
extern void TPLL_SetMI(TPLL_MIVauleTypeDef MI_value);


/**
 * @brief      This function servers to set the length in octet of the preamble
 *             field of the on-air data packet.Note that the valid range is 1-16.
 * @param[in]  preamble_len  specify the length of preamble field in octet.
 * @param[out] none
 * @return     none.
 */
extern void TPLL_Preamble_Set(unsigned char preamble_len);
/**
 * @brief      This function servers to read the length in octet of the preamble
 *             field of the on-air data packet.
 * @return     preamble length.
 */
extern unsigned char TPLL_Preamble_Read(void);


#endif /*_TPLL_H_*/
