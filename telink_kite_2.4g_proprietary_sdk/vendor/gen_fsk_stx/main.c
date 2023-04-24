#include "driver.h"
#include "genfsk_ll.h"

#define    GREEN_LED_PIN       GPIO_PD3
#define    TIME_DEBUG_PIN      GPIO_PD4

static unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};


unsigned char tx_payload[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

volatile static unsigned char tx_done_flag = 0;
 _attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler( void)
{
    unsigned int irq_src = irq_get_src();
    unsigned short rf_irq_src = rf_irq_src_get();

    if (irq_src & FLD_IRQ_ZB_RT_EN) {//if rf irq occurs
        if (rf_irq_src & FLD_RF_IRQ_TX) {//if rf tx irq occurs
            tx_done_flag = 1;
            gpio_toggle(TIME_DEBUG_PIN);
        }
        rf_irq_clr_src(FLD_RF_IRQ_ALL);
    }
    irq_clr_src2(FLD_IRQ_ALL);
}

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
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, sizeof(tx_payload));
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_channel_set(7); //set rf freq as 2403.5MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
    //irq configuration
    rf_irq_enable(FLD_RF_IRQ_TX); //enable rf tx irq
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    irq_enable(); //enable general irq

    //fill the DMA tx buffer
    tx_buffer[0] = sizeof(tx_payload);
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    memcpy(&tx_buffer[4], tx_payload, sizeof(tx_payload));

    while (1) {
        tx_done_flag = 0;
        gen_fsk_stx_start(tx_buffer, clock_time()+100*16);
        gpio_toggle(TIME_DEBUG_PIN);
        while (tx_done_flag == 0);
        
        gpio_write(GREEN_LED_PIN, 1); //LED On
        WaitMs(100);
        gpio_write(GREEN_LED_PIN, 0); //LED Off

        tx_buffer[4]++;
    }
}
