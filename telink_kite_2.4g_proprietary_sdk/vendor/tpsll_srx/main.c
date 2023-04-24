/********************************************************************************************************
 * @file	main.c
 *
 * @brief	This is the source file for 8355
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

#include "driver.h"
#include "tpsll.h"

#define RX_BUF_SIZE                     252
volatile unsigned char tpsll_rxbuf[RX_BUF_SIZE]  __attribute__((aligned(4)));

#define GREEN_LED_PIN       GPIO_PD3
#define DEBUG_PIN           GPIO_PC6

//RX Buffer related
volatile static unsigned char chn = 10;
volatile static unsigned char rx_flag = 0;
volatile static unsigned char rx_first_timeout = 0;
volatile static unsigned int rx_test_cnt = 0;
volatile static unsigned int rx_first_timeout_cnt = 0;
volatile static unsigned char *rx_payload = 0;
volatile static unsigned char rx_payload_len = 0;
volatile static unsigned char rssi = 0;
volatile static unsigned char rssi_ins = 0;
volatile static unsigned int rx_timestamp = 0;
volatile static unsigned char payload_len = 0;

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os")))void irq_handler (void)
{
    unsigned int irq_src = irq_get_src();
    unsigned short rf_irq_src = rf_irq_src_get();

    if (irq_src & FLD_IRQ_ZB_RT_EN) {//if rf irq occurs
    	if (rf_irq_src & FLD_RF_IRQ_RX) {//if rf rx irq occurs
			rf_irq_clr_src(FLD_RF_IRQ_RX);

			rx_test_cnt++;

			if (tpsll_is_rx_crc_ok(tpsll_rxbuf)) {
				rx_flag = 1;
			}

		}
        if (rf_irq_src & FLD_RF_IRQ_FIRST_TIMEOUT) {//if rf tx irq occurs
			rf_irq_clr_src(FLD_RF_IRQ_FIRST_TIMEOUT);
			rx_first_timeout = 1;
			rx_first_timeout_cnt++;
            }
    }
    irq_clr_src2(FLD_IRQ_ALL);
}

int main(void)
{
    
	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN, 0); //LED Off

    gpio_set_func(DEBUG_PIN, AS_GPIO);
    gpio_set_output_en(DEBUG_PIN, 1); //enable output
    gpio_write(DEBUG_PIN, 0); //LED Off

    unsigned char sync_word[4] = {0x11, 0x22, 0x33, 0x44};
    //init Link Layer configuratioin
    tpsll_init(TPSLL_DATERATE_2M);
    tpsll_channel_set(chn);
    tpsll_preamble_len_set(2);
    tpsll_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    tpsll_sync_word_set(TPSLL_PIPE0,sync_word);
    tpsll_pipe_open(TPSLL_PIPE0);
    tpsll_tx_pipe_set(TPSLL_PIPE0);
    tpsll_rx_buffer_set((unsigned char *)tpsll_rxbuf,RX_BUF_SIZE);
    tpsll_radio_power_set(TPSLL_RADIO_POWER_6DBM);
    tpsll_rx_settle_set(90);

    //irq configuration
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_RX | FLD_RF_IRQ_FIRST_TIMEOUT); //enable rf rx and rx first timeout irq
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    irq_enable(); //enable general irq

    //start the SRX
    tpsll_srx_start(clock_time()+50*16, 0); //RX first timeout is disabled and the transceiver won't exit the RX state until a packet arrives

    while (1) {

        if (rx_flag) {
            rx_flag = 0;
            //get rx_payload
            rx_payload = tpsll_rx_payload_get((unsigned char *)tpsll_rxbuf, (unsigned char *)&rx_payload_len);
            rssi = (tpsll_rx_packet_rssi_get((unsigned char *)tpsll_rxbuf) + 110);
            rssi_ins = (tpsll_rx_instantaneous_rssi_get() + 110);
            rx_timestamp = tpsll_rx_timestamp_get((unsigned char *)tpsll_rxbuf);

            //start the SRX
            gpio_toggle(GREEN_LED_PIN);
            tpsll_srx_start(clock_time()+50*16, 500);
            gpio_write(DEBUG_PIN,1);
            gpio_write(DEBUG_PIN,0);
            tpsll_rx_payload_get(tpsll_rxbuf,&payload_len);
        }
        if (rx_first_timeout) {
            rx_first_timeout = 0;
            //start the SRX
            tpsll_srx_start(clock_time()+50*16, 500);
        }
    }
}
