#include "driver.h"
#include "genfsk_ll.h"

#define    BLUE_LED_PIN       GPIO_PD2
#define    GREEN_LED_PIN      GPIO_PD3
#define    WHITE_LED_PIN      GPIO_PD4
#define    RED_LED_PIN        GPIO_PD5
#define    TIME_DEBUG_PIN     GPIO_PC7
#define    FUNC_STX_TEST             1
#define    FUNC_SRX_TEST             2
#define    FUNC_STX2RX_TEST          3
#define    FUNC_SRX2TX_TEST          4

#define    FUNC_TEST         FUNC_SRX2TX_TEST

volatile unsigned char rx_ptr = 0;
volatile unsigned char rx_flag = 0;
volatile unsigned char rx_first_timeout = 0;
volatile unsigned char rx_payload_len = 0;
volatile unsigned int rx_test_cnt = 0;
volatile unsigned char tx_done_flag = 0;
volatile unsigned char *rx_packet = 0;
volatile unsigned char *rx_payload = 0;
volatile unsigned char rx_timeout_flag = 0;
volatile unsigned char rssi = 0;
volatile unsigned int rx_timestamp = 0;
volatile unsigned int t0;

void func_stx_test(void);
void func_srx_test(void);
void func_stx2rx_test(void);
void func_srx2tx_test(void);

//RX Buffer related
#define    RX_BUF_LEN                     64
#define    RX_BUF_NUM                     4
 volatile unsigned char rx_buf[RX_BUF_LEN*RX_BUF_NUM] __attribute__ ((aligned (4))) = {};

//TX Buffer related
unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};
unsigned char tx_payload[8] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xaa, 0xbb};

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler(void)
{
    unsigned int irq_src = irq_get_src();
    unsigned short rf_irq_src = rf_irq_src_get();

if (irq_src & FLD_IRQ_ZB_RT_EN) {//if rf irq occurs

    	if (rf_irq_src & FLD_RF_IRQ_TX)
    	{//if rf tx irq occurs
    		rf_irq_clr_src(FLD_RF_IRQ_TX);
    		tx_done_flag = 1;
    		rf_pa_handler(FLD_RF_IRQ_TX);
    		gpio_toggle(GPIO_PA2);
    	}
    	if (rf_irq_src & FLD_RF_IRQ_RX) {//if rf rx irq occurs
			rf_irq_clr_src(FLD_RF_IRQ_RX);
			rx_test_cnt++;
			rx_packet = rx_buf + rx_ptr*RX_BUF_LEN;

			//set to next rx_buf
			rx_ptr = (rx_ptr + 1) % RX_BUF_NUM;
			gen_fsk_rx_buffer_set((unsigned char *)(rx_buf + rx_ptr*RX_BUF_LEN), RX_BUF_LEN);

			if (gen_fsk_is_rx_crc_ok(rx_packet)) {
				rx_flag = 1;
				rf_pa_handler(FLD_RF_IRQ_RX);
				gpio_toggle(GPIO_PA3);
			}
    	}
       if (rf_irq_src & FLD_RF_IRQ_FIRST_TIMEOUT) {//if rf rx irq occurs
			rf_irq_clr_src(FLD_RF_IRQ_FIRST_TIMEOUT);
			rf_pa_handler(FLD_RF_IRQ_FIRST_TIMEOUT);
			rx_first_timeout = 1;
			gpio_toggle(GPIO_PA4);
		}
       if (rf_irq_src & FLD_RF_IRQ_RX_TIMEOUT) {//if rf tx irq occurs
		   rf_irq_clr_src(FLD_RF_IRQ_RX_TIMEOUT);
		   rf_pa_handler(FLD_RF_IRQ_RX_TIMEOUT);
		   rx_timeout_flag = 1;
		   gpio_toggle(GPIO_PC6);
		}
}
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
    irq_clr_src2(FLD_IRQ_ALL);
}

int main(void)
{
    unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};

	cpu_wakeup_init();

	user_read_flash_value_calib();

	usb_loginit(); //config the USB Log function

    clock_init(SYS_CLK_24M_Crystal);

    rf_pa_init();
    //USB interface config
    gpio_set_func(GPIO_PA5, AS_USB); //config GPIO_PA5 as USB DM pin
    gpio_set_func(GPIO_PA6, AS_USB); //config GPIO_PA6 as USB DP pin
    usb_dp_pullup_en(1); //enable the pull-up of DP pin

    //LED pin config
    gpio_set_func(GREEN_LED_PIN|BLUE_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN|BLUE_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN|BLUE_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 0); //LED Off

    gpio_set_func(GPIO_PA2|GPIO_PA3|GPIO_PA4, AS_GPIO);
    gpio_set_output_en(GPIO_PA2|GPIO_PA3|GPIO_PA4, 1); //enable output
    gpio_write(GPIO_PA2|GPIO_PA3|GPIO_PA4, 0); //LED Off

    gpio_set_func(TIME_DEBUG_PIN|GPIO_PC6, AS_GPIO);
    gpio_set_output_en(TIME_DEBUG_PIN|GPIO_PC6, 1);
    gpio_write(TIME_DEBUG_PIN|GPIO_PC6, 0);

    //generic FSK Link Layer configuratioin
    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, 8);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_rx_buffer_set(rx_buf + rx_ptr*RX_BUF_LEN, RX_BUF_LEN);
    gen_fsk_channel_set(7); //set rf freq as 2403.5MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO);
    gen_fsk_rx_settle_set(89);
    //irq configuration
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_RX | FLD_RF_IRQ_TX|FLD_RF_IRQ_RX_TIMEOUT|FLD_RF_IRQ_FIRST_TIMEOUT);
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    irq_enable(); //enable general irq

    //fill the DMA tx buffer
    tx_buffer[0] = sizeof(tx_payload);
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    memcpy(&tx_buffer[4], tx_payload, sizeof(tx_payload));


    while (1)
    {
    	WaitMs(1000);
#if (FUNC_TEST == FUNC_STX_TEST)
    		func_stx_test();
#elif (FUNC_TEST == FUNC_SRX_TEST)
    		rx_flag = 0;
    		rx_first_timeout = 0;
    		gen_fsk_srx_start(clock_time()+50*16, 0); //RX first timeout is disabled and the transceiver won't exit the RX state until a packet arrives
    		func_srx_test();
#elif (FUNC_TEST == FUNC_STX2RX_TEST)
    		rx_timeout_flag = 0;
    		rx_flag = 0;
    		gen_fsk_stx2rx_start(tx_buffer, clock_time()+50*16, 250);
    		func_stx2rx_test();
#elif (FUNC_TEST == FUNC_SRX2TX_TEST)
    		rx_flag = 0;
    		tx_done_flag = 0;
    		gen_fsk_srx2tx_start(tx_buffer, clock_time()+50*16, 0);
    		func_srx2tx_test();
#endif
    	}

}

void func_stx_test(void)
{
	//the STX
	t0= clock_time();
	while(!clock_time_exceed(t0,1000000))
	{
	tx_done_flag = 0;
	gen_fsk_stx_start(tx_buffer, clock_time()+100*16);
	gpio_toggle(TIME_DEBUG_PIN);
	while (tx_done_flag == 0);

	gpio_toggle(RED_LED_PIN);
	WaitMs(100); //delay 500 ms
	tx_buffer[4]++;
	}
}

void func_srx_test(void)
{
	//the SRX
	//start the SRX

	t0= clock_time();
	while(!clock_time_exceed(t0,1000000))
	{
			if (rx_flag) {
				rx_flag = 0;
				rx_payload = gen_fsk_rx_payload_get(rx_packet, &rx_payload_len);
				rssi = (gen_fsk_rx_packet_rssi_get(rx_packet) + 110);
				rx_timestamp = gen_fsk_rx_timestamp_get(rx_packet);

				gpio_toggle(WHITE_LED_PIN);


				log_msg(0, rx_payload, rx_payload_len);
				log_msg(0, &rssi, 1);
				log_msg(0, &rx_timestamp, 4);
				//start the SRX
				gen_fsk_srx_start(clock_time()+50*16, 120);
				gpio_toggle(TIME_DEBUG_PIN);
			}

			if (rx_first_timeout) {
				rx_first_timeout = 0;
				gen_fsk_srx_start(clock_time()+50*16, 120);
				gpio_toggle(TIME_DEBUG_PIN);
			}
	}
}

void func_stx2rx_test(void)
{
	//the STX2RX
	t0= clock_time();
	while(!clock_time_exceed(t0,1000000))
	{
			if (rx_timeout_flag) {
				rx_timeout_flag = 0;

				WaitMs(100);
				gen_fsk_stx2rx_start(tx_buffer, clock_time()+50*16, 250);
			}

			if (rx_flag) {
				rx_flag = 0;
				rx_payload = gen_fsk_rx_payload_get(rx_packet, &rx_payload_len);
				rssi = (gen_fsk_rx_packet_rssi_get(rx_packet) + 110);
				rx_timestamp = gen_fsk_rx_timestamp_get(rx_packet);

				gpio_toggle(GREEN_LED_PIN);

				WaitMs(100);
				gen_fsk_stx2rx_start(tx_buffer, clock_time()+50*16, 250);
				gpio_toggle(TIME_DEBUG_PIN);
			}
	}
}

void func_srx2tx_test(void)
{
	//the SRX2TX
	t0= clock_time();
	while(!clock_time_exceed(t0,1000000))
	{
		if (rx_flag) {

			rx_flag = 0;
			rx_payload = gen_fsk_rx_payload_get(rx_packet, &rx_payload_len);
			rssi = (gen_fsk_rx_packet_rssi_get(rx_packet) + 110);
			rx_timestamp = gen_fsk_rx_timestamp_get(rx_packet);

			gpio_toggle(BLUE_LED_PIN);

			log_msg(0, rx_payload, rx_payload_len);
			log_msg(0, &rssi, 1);
			log_msg(0, &rx_timestamp, 4);

		}

		if (tx_done_flag) {
			tx_done_flag = 0;
			//start the SRX2TX
			gen_fsk_srx2tx_start(tx_buffer, clock_time()+50*16, 0);
			gpio_toggle(TIME_DEBUG_PIN);

		}
	}

}



