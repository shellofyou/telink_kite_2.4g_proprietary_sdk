#include "driver.h"
#include "common.h"

#define    GREEN_LED_PIN       GPIO_PD3

volatile unsigned int test_result[32];

int main(void)
{
    int i = 0;

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //RNG module initialize
    random_generator_init();

    for (i = 0; i < 32; i++) {
        test_result[i] = rand();
    }

    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN, 0); //LED Off

    while (1) {
        gpio_toggle(GREEN_LED_PIN);
        WaitMs(500);
    }

    return 0;
}



