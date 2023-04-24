#include "driver.h"

#define    GREEN_LED_PIN       GPIO_PD3

volatile unsigned char  ble_tx_packet[256] __attribute__ ((aligned (4))) = {};
volatile unsigned char payload[256] = {	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
										0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
										0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
										0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};

static void User_Init(void)
{

	unsigned int access_code = 0xaabbccdd;
	unsigned char channel = 35;
    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN, 0); //LED Off

	rf_drv_init(RF_MODE_BLE_1M_NO_PN);
	rf_set_power_level_index (RF_POWER_P10p46dBm);
	rf_set_tx_rx_off();
	rf_set_channel(channel,0);
	rf_access_code_comm(access_code);
	rf_set_txmode();
}
static void send_data(unsigned char *pdata,unsigned char pheader,unsigned char len)
{
	ble_tx_packet[0] = len +2; //
	ble_tx_packet[1] = 0x00;   // tx packet length

	ble_tx_packet[2] = 0x00;
	ble_tx_packet[3] = 0x00;

	ble_tx_packet[4] = pheader; //header
	ble_tx_packet[5] = len; //payload length
	memcpy(&ble_tx_packet[6], pdata, len);

	rf_tx_pkt (ble_tx_packet);
}

volatile unsigned char tx_len = 10;
volatile unsigned char header = 0xaa;
int main(void)
{

	cpu_wakeup_init();
	user_read_flash_value_calib();
    clock_init(SYS_CLK_24M_Crystal);
    User_Init();
    while(1)
    {
    	WaitMs(500);
    	gpio_toggle(GREEN_LED_PIN);
    	send_data(payload,header,tx_len);
		while(!rf_tx_finish());
		rf_tx_finish_clear_flag();
		tx_len = (tx_len+1)%20;
		if(!tx_len)
			tx_len = 10;
    }
}
