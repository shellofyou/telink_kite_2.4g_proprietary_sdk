#include "driver.h"
#include "common.h"

/* Define system clock */
#define CLOCK_SYS_CLOCK_HZ  	24000000		// define system clock


/* List tick per second/millisecond/microsecond */
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,				///< system tick per 1 second
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),		///< system tick per 1 millisecond
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),   ///< system tick per 1 microsecond
};



#define    GREEN_LED_PIN        GPIO_PD3


void user_init()
{
	WaitMs(1000);  //leave enough time for SWS_reset when power on

	wd_set_interval_ms(1000,CLOCK_SYS_CLOCK_1MS);
	wd_start();

}





volatile unsigned char debug_cnt=20;

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


    while(debug_cnt--)
    {
    WaitMs(50);
	gpio_toggle(GREEN_LED_PIN);
    }


    //wd_clear();
}





