/********************************************************************************************************
 * @file	rf_pa.c
 *
 * @brief	This is the source file for B85
 *
 * @author	Driver Group
 * @date	May 8,2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "rf_pa.h"
#include "gpio.h"
#include "compiler.h"
#include "genfsk_ll.h"

_attribute_data_retention_	rf_pa_callback_t  rf_pa_cb = 0;


_attribute_ram_code_sec_ void app_rf_pa_handler(pa_type type)
{
#if(PA_ENABLE)
	if(type == PA_TYPE_TX_ON){
	    gpio_set_output_en(PA_RXEN_PIN, 0);
	    gpio_write(PA_RXEN_PIN, 0);
	    gpio_set_output_en(PA_TXEN_PIN, 1);
	    gpio_write(PA_TXEN_PIN, 1);
}
	else if(type == PA_TYPE_RX_ON){
	    gpio_set_output_en(PA_RXEN_PIN, 1);
	    gpio_write(PA_RXEN_PIN, 1);
	    gpio_set_output_en(PA_TXEN_PIN, 0);
	    gpio_write(PA_TXEN_PIN, 0);
	}
	else{
	    gpio_set_output_en(PA_RXEN_PIN, 0);
	    gpio_write(PA_RXEN_PIN, 0);
	    gpio_set_output_en(PA_TXEN_PIN, 0);
	    gpio_write(PA_TXEN_PIN, 0);
	}
#endif
}


void rf_pa_init(void)
{
#if(PA_ENABLE)
    //rf_set_power_level_index (RF_POWER_0dBm);
    gpio_set_func(PA_TXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);

    gpio_set_func(PA_RXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);

    rf_pa_cb = app_rf_pa_handler;
#endif
}

_attribute_ram_code_sec_ void rf_pa_handler(short int flag)
{

	if (!rf_pa_cb)
		return;

	switch(flag){
	case FLD_RF_IRQ_TX:
				if (gen_fsk_current_mode == GEN_FSK_MD_STX || gen_fsk_current_mode == GEN_FSK_MD_SRX2TX)
				{
					rf_pa_cb(PA_TYPE_OFF);
				}
				else if (gen_fsk_current_mode == GEN_FSK_MD_STX2RX)
				{
					rf_pa_cb(PA_TYPE_RX_ON);
				}
                break;
	case FLD_RF_IRQ_RX:
				if(gen_fsk_current_mode == GEN_FSK_MD_SRX || gen_fsk_current_mode == GEN_FSK_MD_STX2RX)
				{
					rf_pa_cb(PA_TYPE_OFF);
				}
				if (gen_fsk_current_mode == GEN_FSK_MD_SRX2TX )
				{
					rf_pa_cb(PA_TYPE_TX_ON);
				}
                break;
	case FLD_RF_IRQ_FIRST_TIMEOUT:
				if (gen_fsk_current_mode == GEN_FSK_MD_SRX)
				{
					  rf_pa_cb(PA_TYPE_OFF);
				}
                break;
	case FLD_RF_IRQ_RX_TIMEOUT:
				if (gen_fsk_current_mode == GEN_FSK_MD_STX2RX)
				{
					  rf_pa_cb(PA_TYPE_OFF);
				}
                break;
	default:
		break;
}


}


