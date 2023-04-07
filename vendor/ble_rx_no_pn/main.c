#include "driver.h"
#include "tpll.h"

#define    GREEN_LED_PIN       GPIO_PD3

#define    RX_BUF_LEN                     64
#define    RX_BUF_NUM                     4
volatile static unsigned char rx_buf[RX_BUF_LEN*RX_BUF_NUM] __attribute__ ((aligned (4))) = {};
volatile static unsigned char rx_ptr = 0;
volatile static unsigned char *rx_packet = 0;
volatile static unsigned char pkt_header = 0;
volatile static unsigned char pkt_len = 0;

volatile unsigned char rx_flag = 0;

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler(void)
{
	unsigned int irq_src = irq_get_src();
	unsigned short rf_irq_src = rf_irq_src_get();

	 if (irq_src & FLD_IRQ_ZB_RT_EN)//if rf irq occurs
	 {
	        if (rf_irq_src & FLD_RF_IRQ_RX) //if rf rx irq occurs
	        {
	        	rx_packet = rx_buf + rx_ptr*RX_BUF_LEN;


				rx_ptr = (rx_ptr + 1) % RX_BUF_NUM;
				rf_rx_buffer_set((rx_buf + rx_ptr*RX_BUF_LEN), RX_BUF_LEN,0);

				if(RF_BLE_PACKET_LENGTH_OK(rx_packet))
				{
					rx_flag = 1;
					pkt_header = rx_packet[4];
					pkt_len = rx_packet[5];
				}
				rf_set_tx_rx_off_auto_mode();
	        }
	        rf_irq_clr_src(FLD_RF_IRQ_ALL);
	    }
	    irq_clr_src2(FLD_IRQ_ALL);
}




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

	rf_rx_buffer_set(rx_buf + rx_ptr*RX_BUF_LEN,RX_BUF_LEN, 0);
	rf_set_rxmode ();

	rf_irq_enable(FLD_RF_IRQ_RX); //enable rf tx irq
	irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
	irq_enable(); //enable general irq
}


int main(void)
{

	cpu_wakeup_init();
	user_read_flash_value_calib();
    clock_init(SYS_CLK_24M_Crystal);
    User_Init();
    while(1)
    {
    	if(rx_flag)
    	{
    		gpio_toggle(GREEN_LED_PIN);
    		rx_flag = 0;
    	}
    }
}
