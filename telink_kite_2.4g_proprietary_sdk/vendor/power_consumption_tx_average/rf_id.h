/********************************************************************************************************
 * @file     rf_id.h
 *
 * @brief    This is a RFID reference.
 *
 * @author   jian.zhang@telink-semi.com
 * @date     Mar. 11, 2019
 *
 * @par      Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd.
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
#ifndef _RF_ID_H_
#define _RF_ID_H_

/**
 * @brief       This function initializes the radio module for RFID 
 *              
 * @param[in]   none 
 * @param[out]  none
 * @return      none
 *              
 */
extern void rfid_radio_config(void);

/**
 * @brief       This function servers to send an on-air data packet containing specified payload.
 *              
 * @param[in]   payload pointer to the payload needs sending out
 * @param[in]   payload_len length of the payload in octet
 * @param[out]  none
 * @return      none
 *              
 */
extern void rfid_packet_send(unsigned char *payload, unsigned char payload_len);

#endif