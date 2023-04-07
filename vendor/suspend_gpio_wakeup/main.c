#include "driver.h"

#define    WAKEUP_PIN          GPIO_PB4
#define    KEY2_GND_PIN        GPIO_PB2
#define    GREEN_LED_PIN       GPIO_PD3

int main(void)
{
//	blc_pm_select_external_32k_crystal();
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_set_output_en(WAKEUP_PIN, 0); //disable output
    gpio_set_input_en(WAKEUP_PIN, 0); //disable input
    gpio_setup_up_down_resistor(WAKEUP_PIN, PM_PIN_PULLUP_1M); //enable internal 1M pull-up
    cpu_set_gpio_wakeup(WAKEUP_PIN, Level_Low, 1); //config low-level wakeup

    gpio_set_output_en(KEY2_GND_PIN, 0); //disable output
    gpio_set_input_en(KEY2_GND_PIN, 0); //disable input
    gpio_setup_up_down_resistor(KEY2_GND_PIN, PM_PIN_PULLDOWN_100K); //enable internal 100k pull-down

    while (1) {
    	gpio_shutdown(GPIO_ALL); //set all GPIO as high_Z state, avoiding current leakage
        cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_PAD, 0);
    
        //led pin config
        gpio_set_func(GREEN_LED_PIN, AS_GPIO);
        gpio_set_output_en(GREEN_LED_PIN, 1);
        gpio_write(GREEN_LED_PIN, 1);
        WaitMs(3000);
    }
}
