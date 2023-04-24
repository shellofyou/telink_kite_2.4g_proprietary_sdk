#include "driver.h"
#include "common.h"


#define    GREEN_LED_PIN      GPIO_PD3

void user_init()
{
	WaitMs(1000);  //leave enough time for SWS_reset when power on

	timer2_set_mode(TIMER_MODE_TICK,0,0);

	timer_start(TIMER2);

}

volatile unsigned int timer2_tick_value=0;


_attribute_ram_code_sec_noinline_ int main (void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();



	gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
	gpio_set_input_en(GREEN_LED_PIN, 0); //disable input
	gpio_write(GREEN_LED_PIN, 0); //LED Off


	while (1) {

		timer2_tick_value = reg_tmr2_tick;
		WaitMs(50);
		gpio_toggle(GREEN_LED_PIN);

	  }


}



