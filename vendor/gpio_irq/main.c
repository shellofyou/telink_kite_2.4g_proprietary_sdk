#include "driver.h"

#define GREEN_LED_PIN     		GPIO_PD3
#define IRQ_PIN           		GPIO_PD0
#define IRQ_KEY           		GPIO_PB4
#define GPIO_IRQ				1
#define GPIO_IRQ_KEY			2//SW2
#define GPIO_IRQ_RSIC0			3
#define GPIO_IRQ_RSIC1			4
#define GPIO_HIGH_RESISTOR		5

#define GPIO_MODE 				GPIO_IRQ_RSIC1

volatile unsigned int gpio_irq_cnt;
volatile unsigned int gpio_set_irq_cnt;
volatile unsigned char gpio_irq_flag = 0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
#if (GPIO_MODE == GPIO_IRQ )
    unsigned int irq_src = irq_get_src();
	if (irq_src & FLD_IRQ_GPIO_EN)
	{
		reg_irq_src |= FLD_IRQ_GPIO_EN; // clear the relevant irq
	    if (0 == gpio_read(IRQ_PIN))
	    {
	        WaitMs(10);
	        if (0 == gpio_read(IRQ_PIN))
	        {
	            while (0 == gpio_read(IRQ_PIN));
	            gpio_irq_cnt++;
	            gpio_toggle(GREEN_LED_PIN);
	        }
	    }
	}
#elif (GPIO_MODE == GPIO_IRQ_KEY )
    unsigned int irq_src = irq_get_src();
	if (irq_src & FLD_IRQ_GPIO_EN)
	{
		reg_irq_src |= FLD_IRQ_GPIO_EN; // clear the relevant irq
	    if (0 == gpio_read(IRQ_KEY))
	    {
	        WaitMs(10);
	        if (0 == gpio_read(IRQ_KEY))
	        {
	            while (0 == gpio_read(IRQ_KEY));
	            gpio_irq_cnt++;
	            gpio_toggle(GREEN_LED_PIN);
	        }
	    }
	}
#elif(GPIO_MODE == GPIO_IRQ_RSIC0)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC0_EN)==FLD_IRQ_GPIO_RISC0_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC0_EN; // clear the relevant irq
		gpio_irq_cnt++;
		gpio_toggle(GREEN_LED_PIN);
	}
#elif(GPIO_MODE == GPIO_IRQ_RSIC1)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC1_EN)==FLD_IRQ_GPIO_RISC1_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC1_EN; // clear the relevant irq
		gpio_irq_cnt++;
		gpio_toggle(GREEN_LED_PIN);

	}
#endif
}


void gpio_test_init(void)
{
	/*
		 * Button matrix table:
		 * 			PB4  	PB5
		 * 	PB2	    SW2		SW3
		 * 	PB3   	SW4		SW5
		 */
		gpio_set_func(GPIO_PB2 ,AS_GPIO);
		gpio_set_output_en(GPIO_PB2, 1); 		//enable output
		gpio_set_input_en(GPIO_PB2 ,0);			//disable input
		gpio_write(GPIO_PB2, 0);

		//2.init the KEY1 pin,for trigger interrupt
		gpio_set_func(IRQ_KEY ,AS_GPIO);
		gpio_set_output_en(IRQ_KEY, 0); 			// disable output
		gpio_set_input_en(IRQ_KEY ,1);				// enable input

		gpio_set_func(GREEN_LED_PIN, AS_GPIO);
		gpio_set_output_en(GREEN_LED_PIN, 1);         //enable output
		gpio_set_input_en(GREEN_LED_PIN, 0);          //disable input
		gpio_write(GREEN_LED_PIN, 1);                 //LED On

		/****  IRQ pin link PD1, PD1 produces a square wave.  **/
		gpio_set_func(GPIO_PD1 ,AS_GPIO);
		gpio_set_output_en(GPIO_PD1, 1); 		//enable output
		gpio_set_input_en(GPIO_PD1 ,0);			//disable input
		gpio_write(GPIO_PD1, 0);

		//2.init the IRQ pin, for trigger interrupt
		gpio_set_func(IRQ_PIN ,AS_GPIO);
		gpio_set_output_en(IRQ_PIN, 0); 			// disable output
		gpio_set_input_en(IRQ_PIN ,1);				// enable input
}

void user_init(void)
{
#if(GPIO_MODE == GPIO_HIGH_RESISTOR)
	gpio_shutdown(GPIO_ALL);				//set all gpio as high resistor except sws and mspi
#else

	gpio_test_init();

#if (GPIO_MODE == GPIO_IRQ )
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ POL_FALLING   Press SW2 to connect KEY1 and KEY3 to trigger an interrupt. **/
	gpio_set_interrupt(IRQ_PIN, POL_FALLING);	//When SW2 is pressed, the falling edge triggers the interrupt.
	irq_enable();
#elif (GPIO_MODE == GPIO_IRQ_KEY )
	gpio_setup_up_down_resistor(IRQ_KEY, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ POL_FALLING   Press SW2 to connect KEY1 and KEY3 to trigger an interrupt. **/
	gpio_set_interrupt(IRQ_KEY, POL_FALLING);	//When SW2 is pressed, the falling edge triggers the interrupt.
	irq_enable();
#elif(GPIO_MODE == GPIO_IRQ_RSIC0)
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLDOWN_100K);
	/****GPIO_IRQ_RSIC0  POL_RISING   toggle PD1 to trigger an interrupt. **/
	gpio_set_interrupt_risc0(IRQ_PIN, POL_RISING);	//When switching PD1, the rising edge triggers an interrupt.
	irq_enable();

#elif(GPIO_MODE == GPIO_IRQ_RSIC1)
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ_RSIC1  POL_FALLING   toggle PD1 to trigger an interrupt. **/
	gpio_set_interrupt_risc1(IRQ_PIN, POL_FALLING);	//When switching PD1, the falling edge triggers an interrupt.
	irq_enable();
#endif
#endif
}

int main(void)
{
	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();
    while (1)
    {
	   sleep_ms(10);
	   gpio_toggle(GPIO_PD1);
    }
	return 0;
}
