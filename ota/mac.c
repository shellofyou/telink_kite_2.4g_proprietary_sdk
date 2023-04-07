/********************************************************************************************************
 * @file	mac.c
 *
 * @brief	This is the source file for 825x
 *
 * @author	2.4G Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#include "mac.h"
#include "driver.h"
#include "common.h"
#include "genfsk_ll.h"

#define MAC_TX_BUF_LEN                64
#define MAC_RX_BUF_LEN                64
#define MAC_RX_BUF_NUM                4
#define MAC_STX_WAIT                  30 //in us
#define MAC_SRX_WAIT                  5  //in us
#define MAC_RX_WAIT                   200000 //in us

#define MAC_RX_PACKET_LENGTH_OK(p)    (p[0] == p[12]+13)
#define MAC_RX_PACKET_CRC_OK(p)       ((p[p[0]+3] & 0x51) == 0x10)

typedef struct {
    unsigned short Channel;
    MAC_Cb RxCb;
    MAC_Cb RxTimeoutCb;
    MAC_Cb RxFirstTimeoutCb;
} MAC_InfoTypeDef;

MAC_InfoTypeDef mac_Info = {0, 0, 0};

static unsigned char mac_TxBuf[MAC_TX_BUF_LEN] __attribute__ ((aligned (4))) = {};
static unsigned char mac_RxBuf[MAC_RX_BUF_LEN*MAC_RX_BUF_NUM] __attribute__ ((aligned (4))) = {};
static unsigned char mac_RxPtr = 0;

void MAC_Init(const unsigned short Channel,
              const MAC_Cb RxCb,
              const MAC_Cb RxTimeoutCb,
              const MAC_Cb RxFirstTimeoutCb)
{
    mac_Info.Channel = Channel;
    mac_Info.RxCb = RxCb;
    mac_Info.RxTimeoutCb = RxTimeoutCb;
    mac_Info.RxFirstTimeoutCb = RxFirstTimeoutCb;

    unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
    //generic FSK Link Layer configuratioin
    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_VARIABLE_PAYLOAD, 8);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);

    gen_fsk_rx_buffer_set(mac_RxBuf + mac_RxPtr*MAC_RX_BUF_LEN , MAC_RX_BUF_LEN);
    gen_fsk_channel_set(Channel);

    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
    WaitUs(130); //wait for tx settle

    //irq configuration
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_RX | FLD_RF_IRQ_TX | FLD_RF_IRQ_RX_TIMEOUT|FLD_RF_IRQ_FIRST_TIMEOUT);
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    irq_enable(); //enable general irq
}

void MAC_SendData(const unsigned char *Payload,
                 const int PayloadLen)
{
    mac_TxBuf[0] = PayloadLen + 1;
    mac_TxBuf[1] = 0x00;
    mac_TxBuf[2] = 0x00;
    mac_TxBuf[3] = 0x00;
    mac_TxBuf[4] = PayloadLen;
    memcpy(&mac_TxBuf[5], Payload, PayloadLen); //payload
    gen_fsk_stx2rx_start(mac_TxBuf, clock_time()+MAC_STX_WAIT*16, MAC_RX_WAIT);
}

void MAC_RecvData(unsigned int TimeUs)
{
	gen_fsk_srx_start(clock_time()+MAC_SRX_WAIT*16, TimeUs);
}

void MAC_RxIrqHandler(void)
{
    //set next rx_buf
    unsigned char *RxPacket = mac_RxBuf + mac_RxPtr*MAC_RX_BUF_LEN;
    mac_RxPtr = (mac_RxPtr + 1) % MAC_RX_BUF_NUM;
    gen_fsk_rx_buffer_set(mac_RxBuf + mac_RxPtr*MAC_RX_BUF_LEN , MAC_RX_BUF_LEN);

    /* clear the interrupt flag */
    reg_rf_irq_status = FLD_RF_IRQ_RX;

    if (!MAC_RX_PACKET_CRC_OK(RxPacket)) {
        if (mac_Info.RxCb) {
            mac_Info.RxCb(NULL);
        }
        return;
    }
    if (mac_Info.RxCb) {
        mac_Info.RxCb(&RxPacket[4]);
    }
}

void MAC_RxTimeOutHandler(void)
{
    /* clear the interrupt flag */
	reg_rf_irq_status = FLD_RF_IRQ_RX_TIMEOUT;

    if (mac_Info.RxTimeoutCb) {
        mac_Info.RxTimeoutCb(NULL);
    }
}

void MAC_RxFirstTimeOutHandler(void)
{
    /* clear the interrupt flag */
	reg_rf_irq_status = FLD_RF_IRQ_FIRST_TIMEOUT;

    if (mac_Info.RxFirstTimeoutCb) {
        mac_Info.RxFirstTimeoutCb(NULL);
    }
}
