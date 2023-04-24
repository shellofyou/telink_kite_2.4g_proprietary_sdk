#include "driver.h"
#include "common.h"


#define    GREEN_LED_PIN         GPIO_PD3
#define    PULSE_OUTPUT_PIN      GPIO_PD0

#define    SAMPLE_INTPUT_PIN      GPIO_PC5

volatile unsigned char timer0_expire_flg = 0;
volatile unsigned char timer1_expire_flg = 0;
volatile unsigned char timer2_expire_flg = 0;

unsigned int timer0_irq_cnt = 0;
unsigned int timer1_irq_cnt = 0;
unsigned int timer2_irq_cnt = 0;

volatile unsigned int timer_pulse_width_tick_value=0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	if(reg_tmr_sta & FLD_TMR_STA_TMR0){
		reg_tmr_sta = FLD_TMR_STA_TMR0; //clear irq status
		timer0_irq_cnt ++;
		timer0_expire_flg = 1;

	}
	if(reg_tmr_sta & FLD_TMR_STA_TMR1){
		reg_tmr_sta = FLD_TMR_STA_TMR1; //clear irq status
		timer1_irq_cnt ++;
		timer1_expire_flg = 1;

	}

	if(reg_tmr_sta & FLD_TMR_STA_TMR2){
		reg_tmr_sta = FLD_TMR_STA_TMR2; //clear irq status
		timer2_irq_cnt ++;;
		timer2_expire_flg = 1;

		reg_tmr_sta |= FLD_TMR_STA_TMR2; //clear irq status
		timer_pulse_width_tick_value = reg_tmr2_tick;
		reg_tmr2_tick = 0;
	}
}


void user_init()
{
	WaitMs(1000);  //leave enough time for SWS_reset when power on


	timer2_gpio_init(SAMPLE_INTPUT_PIN, POL_FALLING);
	irq_enable();
	timer2_set_mode(TIMER_MODE_GPIO_WIDTH,0,0);
	timer_start(TIMER2);

}


_attribute_ram_code_sec_noinline_ int main (void)   //must on ramcode
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();



	gpio_set_output_en(GREEN_LED_PIN|PULSE_OUTPUT_PIN, 1); //enable output
	gpio_set_input_en(GREEN_LED_PIN|PULSE_OUTPUT_PIN, 0); //disable input
	gpio_write(GREEN_LED_PIN|PULSE_OUTPUT_PIN, 0); //LED Off


	while (1) {
		if (timer0_expire_flg) {
			timer0_expire_flg = 0;
			gpio_toggle(GREEN_LED_PIN);
		}
		if (timer1_expire_flg) {
			timer1_expire_flg = 0;
			gpio_toggle(GREEN_LED_PIN);
		}
		if (timer2_expire_flg) {
			timer2_expire_flg = 0;
			gpio_toggle(GREEN_LED_PIN);
		}
		    WaitMs(50);
			gpio_toggle(PULSE_OUTPUT_PIN);
	}
}

