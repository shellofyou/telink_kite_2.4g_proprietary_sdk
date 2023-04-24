#include "driver.h"
#include "common.h"
#include "genfsk_ll.h"
#include "rf_id.h"

#define RF_ID_CHANNEL    7
#define RF_ID_SYNC_WORD  {0x53, 0x78, 0x56, 0x52}

static unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};


_attribute_ram_code_sec_noinline_ void rfid_radio_config(void)
{
    unsigned char sync_word[4] = RF_ID_SYNC_WORD;

    //generic FSK Link Layer configuratioin
    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(2);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_channel_set(RF_ID_CHANNEL); 
    gen_fsk_radio_state_set(GEN_FSK_STATE_TX); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
}

_attribute_ram_code_sec_noinline_ void rfid_packet_send(unsigned char *payload, unsigned char payload_len)
{
    unsigned char i = 0;

    //fill the DMA tx buffer
    tx_buffer[0] = payload_len;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    for (i = 0; i < payload_len; i++) {
        tx_buffer[4+i] = payload[i];
    }
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, payload_len);
    WaitUs(130); //wait for tx settle
    gen_fsk_tx_start(tx_buffer);
    while(0 == gen_fsk_is_tx_done());
    gen_fsk_tx_done_status_clear();
}
