#include "driver.h"
#include "common.h"
#include "rf_id.h"
#include "genfsk_ll.h"

#define    TEST_CHN_NUM         7 //2403.5MHz

static unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
static unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};

_attribute_ram_code_sec_noinline_ void rfid_radio_config(void)
{

    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, 64);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_channel_set(TEST_CHN_NUM); //set rf freq as 2403MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
}

_attribute_ram_code_sec_noinline_ void rfid_packet_send(unsigned char *pdu, unsigned char pdu_len)
{
    unsigned char i = 0;

    //fill the DMA tx buffer
    tx_buffer[0] = pdu_len;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    tx_buffer[4] = pdu[0]; //header0
    tx_buffer[5] = pdu[1]; //header1(payload length) 

    for (i = 0; i < (pdu_len-2); i++) {
        tx_buffer[6+i] = pdu[2+i];
    }
    //gpio_write(DEBUG_PIN, 1);
    //WaitMs(50);
   // rf_start_stx(tx_buffer, clock_time());
    gen_fsk_stx_start(tx_buffer, clock_time());
    cpu_stall(FLD_IRQ_ZB_RT_EN|FLD_IRQ_TMR1_EN, 1200,24);
}
