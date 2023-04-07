#ifndef _BLE_ADV_H_
#define _BLE_ADV_H_


#include "lib/include/rf_drv.h"


/**
 * @brief       This function sets the channel inndex and tx power of BLE advertising packets
 *              with specific 16-Byte key
 * @param[in]   channel_index channel index of the advertising channnel(i.e., 37, 38 or 39)
 * @param[in]   tx_power the rf power to transmit the adv packets
 * @param[out]  none
 * @return      none
 *              
 */
extern void ble_adv_init(unsigned char channel_index, RF_PowerTypeDef tx_power);

/**
 * @brief       This function servers to send the BLE advertising packet
 *              with given pdu.
 * @param[in]   pdu pointer to the buffer containing the pdu needs to be send out. Note
 *              that the pdu includes the two-byte header filed, i.e.,
 *              pdu = header0 + header1(payload length) + payload
 * @param[in]   pdu_len the length of the pdu in octet
 * @param[out]  none
 * @return      none
 *              
 */
extern void ble_adv_send(unsigned char *pdu, unsigned char pdu_len);


#endif /* _BLE_ADV_H_ */
