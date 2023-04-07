#include "driver.h"
#include "common.h"


#define ADC_BASE_MODE			    1	//GPIO voltage
#define ADC_VBAT_MODE			    2	//Battery Voltage
#define ADC_RNG_MODE			    3	//Random number Gernerator,to use this Random mode need set TRNG_MODE_ENABLE=0 in random.h file.

#define ADC_MODE				    ADC_BASE_MODE
#define MANNUAL_MODE_GET_ADC_SAMPLE_RESULT		0	// mannual mode

#define    GREEN_LED_PIN        GPIO_PD3
#define    ADC_INPUT_PIN        GPIO_PB0

volatile unsigned short sample_result[16];

int main (void) 
{
    unsigned char i = 0;

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1);
    gpio_write(GREEN_LED_PIN, 0);
#if(ADC_MODE==ADC_RNG_MODE)
	 random_generator_init();
#else
	adc_init();

	#if(ADC_MODE==ADC_BASE_MODE)
    adc_base_init(ADC_INPUT_PIN);
    adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
    #elif (ADC_MODE==ADC_VBAT_MODE)
		adc_vbat_init(ADC_INPUT_PIN);
	#endif

	adc_power_on_sar_adc(1);		//After setting the ADC parameters, turn on the ADC power supply control bit

#endif

    while (1) {
#if(ADC_MODE==ADC_RNG_MODE)
	rns_val = rand();

#else
        sample_result[i] = adc_sample_and_get_result();
        i = (i+1) % 16;
        WaitMs(50);
        gpio_toggle(GREEN_LED_PIN);

#if(MANNUAL_MODE_GET_ADC_SAMPLE_RESULT==1)
		adc_manual_val = adc_sample_and_get_result_manual_mode();
  #endif
#endif
    }
}
