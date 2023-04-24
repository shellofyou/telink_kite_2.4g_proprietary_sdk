#include "driver.h"

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ      24000000   //  24m
/* List tick per second/millisecond/microsecond */
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,				///< system tick per 1 second
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),		///< system tick per 1 millisecond
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),   ///< system tick per 1 microsecond
};

#define PWM_PIN		            GPIO_PA2
#define AS_PWMx			        AS_PWM0
#define PWM_ID					PWM0_ID
#define PWM_PULSE_NUM			4

#define IR_DMA_CARRIER_FREQ				38000
#define IR_DMA_MAX_TICK					(CLOCK_SYS_CLOCK_HZ/IR_DMA_CARRIER_FREQ)
#define IR_DMA_CMP_TICK					(IR_DMA_MAX_TICK/2)

#define IR_DMA_SHADOW_CARRIER_FREQ		56000
#define IR_DMA_SHADOW_MAX_TICK			(CLOCK_SYS_CLOCK_HZ/IR_DMA_SHADOW_CARRIER_FREQ)
#define IR_DMA_SHADOW_CMP_TICK			(IR_DMA_SHADOW_MAX_TICK/2)

unsigned short IR_DMA_Buff[64]={0};
unsigned short IRQ_IR_DMA_Buff[64]={0};
volatile unsigned char irq_index=2;
volatile unsigned char cnt=0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	if(pwm_get_interrupt_status(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE)){
		pwm_clear_interrupt_status(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
		cnt++;
		IRQ_IR_DMA_Buff[irq_index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
		IRQ_IR_DMA_Buff[irq_index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 1690 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
		unsigned int irq_length = irq_index*2 - 4;//The first four bytes are data length bytes, not included in the actual length to be sent
		unsigned char* irq_buff = &IRQ_IR_DMA_Buff[0];
		//The maximum length that the PWM can send is 511bytes
		irq_buff[0]= irq_length&0xff;
		irq_buff[1]= (irq_length>>8)&0xff;
		irq_buff[2]= (irq_length>>16)&0xff;
		irq_buff[3]= (irq_length>>24)&0xff;
		pwm_set_dma_address(&IRQ_IR_DMA_Buff);
		pwm_start_dma_ir_sending();
		irq_index=2;
	    }
}

void user_init()
{
	sleep_ms(2000);
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(PWM_PIN, AS_PWMx);
	pwm_set_mode(PWM_ID, PWM_IR_DMA_FIFO_MODE);
	pwm_set_cycle_and_duty(PWM_ID, IR_DMA_MAX_TICK, IR_DMA_CMP_TICK);
	pwm_set_pwm0_shadow_cycle_and_duty(IR_DMA_SHADOW_MAX_TICK,IR_DMA_SHADOW_CMP_TICK);
	unsigned char index=2;
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 4500 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 1690 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_MAX_TICK);

	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 9000 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 4500 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 1690 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);
	IR_DMA_Buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 560 * CLOCK_SYS_CLOCK_1US/IR_DMA_SHADOW_MAX_TICK);

	unsigned int length = index*2 - 4;//The first four bytes are data length bytes, not included in the actual length to be sent
	unsigned char* buff = &IR_DMA_Buff[0];
	buff[0]= length&0xff;
	buff[1]= (length>>8)&0xff;
	buff[2]= (length>>16)&0xff;
	buff[3]= (length>>24)&0xff;
	pwm_set_dma_address(&IR_DMA_Buff);

	pwm_set_interrupt_enable(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
	irq_set_mask(FLD_IRQ_SW_PWM_EN);
	irq_enable();

	pwm_start_dma_ir_sending();
}

int  main(void)
{
	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

    gpio_init(0);

	user_init();

	while(1)
	{
		sleep_ms(50);
	}

}


