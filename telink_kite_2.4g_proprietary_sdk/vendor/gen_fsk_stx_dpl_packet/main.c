#include "driver.h"
#include "genfsk_ll.h"

#define    GREEN_LED_PIN       GPIO_PD3
#define    TIME_DEBUG_PIN      GPIO_PC7

static unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};

volatile unsigned char pid = 3;
volatile unsigned char tx_payload_length = 0;

unsigned char tx_payload[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
								0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
								0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
								0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

_attribute_ram_code_sec_noinline_ int main(void)
{


    unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
    

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN, 0); //LED Off

    gpio_set_func(TIME_DEBUG_PIN, AS_GPIO);
    gpio_set_output_en(TIME_DEBUG_PIN, 1); //enable output
    gpio_write(TIME_DEBUG_PIN, 0); //LED Off

    //generic FSK Link Layer configuratioin
    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_VARIABLE_PAYLOAD,0);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_channel_set(7); //set rf freq as 2403.5MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
    gen_fsk_auto_pid_disable();

    tx_payload_length = 5;
    //fill the DMA tx buffer
    tx_buffer[0] = tx_payload_length + 1;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    tx_buffer[4] = tx_payload_length;
    memcpy(&tx_buffer[5], tx_payload, sizeof(tx_payload));

    while (1) {
    	gen_fsk_set_pid(&tx_buffer, pid);
    	gen_fsk_stx_start(tx_buffer, clock_time()+100*16);
    	while(rf_tx_finish() == 0);
    	rf_tx_finish_clear_flag();
    	pid = (pid + 1)&0x03;
    	gpio_toggle(GREEN_LED_PIN);
    	WaitMs(1000);
    }

}


