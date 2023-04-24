#include "driver.h"

#define    DEBUG_IO_PIN        GPIO_PC6
#define    GREEN_LED_PIN       GPIO_PD3
#define    DEEP_DURATION       1000 //unit: ms

_attribute_session_(".retention_data") volatile static unsigned char deep_times_retention = 0;
volatile static unsigned char deep_times_no_retention = 0;



int main(void)
{
//	blc_pm_select_external_32k_crystal();
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);


    if (8 == deep_times_retention) {
        //resume the SWS for debug
        gpio_set_input_en(GPIO_SWS, 1);

        //led pin config
        gpio_set_func(GREEN_LED_PIN, AS_GPIO);
        gpio_set_output_en(GREEN_LED_PIN, 1);
        gpio_write(GREEN_LED_PIN, 0);
        while (1) {
            WaitMs(50);
            gpio_toggle(GREEN_LED_PIN);
        }
    }

    //debug pin config
    gpio_set_func(DEBUG_IO_PIN, AS_GPIO);
    gpio_set_output_en(DEBUG_IO_PIN, 1);
    gpio_write(DEBUG_IO_PIN, 1);
    WaitMs(10);

    deep_times_retention++;
    deep_times_no_retention++;
    gpio_shutdown(GPIO_ALL); //set all unused GPIOs during deep sleep as high_Z state, avoiding current leakage
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_TIMER, ClockTime()+DEEP_DURATION*1000*16);
    
    while (1) {
    
    }
}
