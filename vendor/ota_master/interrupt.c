/********************************************************************************************************
 * @file	interrupt.c
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

#include "driver.h"
#include "common.h"

#define OTA_MASTER_TRIG_PIN    GPIO_PB4

extern volatile unsigned char OTA_MasterTrig;
volatile unsigned char tx_done_cnt, rx_done_cnt, first_timeout_done, rx_timeout_done;
_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler(void)
{

    unsigned int irq_src = irq_get_src();
    unsigned short src_rf = rf_irq_src_get();

    if (irq_src & FLD_IRQ_GPIO_EN) {
    	if (0 == gpio_read(OTA_MASTER_TRIG_PIN)) {
			WaitUs(10);
			if (0 == gpio_read(OTA_MASTER_TRIG_PIN)) {
				while(0 == gpio_read(OTA_MASTER_TRIG_PIN));
				OTA_MasterTrig = 1;
			}
		}
    }

    if (irq_src & FLD_IRQ_ZB_RT_EN) {
        if (src_rf & FLD_RF_IRQ_RX) {
        	rx_done_cnt++;
            MAC_RxIrqHandler();
        }
        if (src_rf & FLD_RF_IRQ_RX_TIMEOUT) {
        	rx_timeout_done++;
            MAC_RxTimeOutHandler();
        }
        if (src_rf & FLD_RF_IRQ_TX) {
        	tx_done_cnt++;
            rf_irq_clr_src(FLD_RF_IRQ_TX);

        }

    }
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
    irq_clr_src();

}



