#include "driver.h"
#include "common.h"
#include "rf_id.h"
#include "genfsk_ll.h"

#define    TEST_CHN_NUM         7 //2403.5MHz
static unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
static unsigned char __attribute__ ((aligned (4))) rx_buffer[64] = {0};

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
      gen_fsk_rx_buffer_set(rx_buffer ,sizeof(rx_buffer));
      gen_fsk_channel_set(TEST_CHN_NUM);
      gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic RX state
      gen_fsk_rx_settle_set(89);



}

_attribute_ram_code_sec_noinline_ void rfid_packet_rx(void)
{

    gen_fsk_srx_start(clock_time(), 0);

    cpu_stall(FLD_IRQ_ZB_RT_EN, 0,24);


}
