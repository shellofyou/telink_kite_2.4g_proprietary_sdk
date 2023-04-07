#include "driver.h"
#include "common.h"

#define    GREEN_LED_PIN        GPIO_PD3

#define 	DBG_DATA_LEN					16
#define 	DBG_DATA_NUM					16


/*************** i2c slave data buffer ****************/
//mapping mode, write buffer is the first half 64 byte, read buffer is last half 64 byte
volatile unsigned char slave_mapping_buff[128] = {0};
unsigned char slave_rx_buff_debug[DBG_DATA_LEN *DBG_DATA_NUM];   //store i2c master writing data here to debug
unsigned int slave_rx_index = 0;


#define IRQ_IIC_CLEAR       0
#define IRQ_IIC_READ  		BIT(0)  //read
#define IRQ_IIC_WRITE		BIT(1)  //write

unsigned int i2c_irq_flag = IRQ_IIC_CLEAR;


unsigned int i2c_read_cnt = 0;   //for debug
unsigned int i2c_write_cnt = 0;  //for debug


_attribute_ram_code_sec_noinline_ void irq_handler(void)
{

	unsigned char  irq_status =i2c_get_interrupt_status(FLD_HOST_CMD_IRQ|FLD_HOST_READ_IRQ);//i2c slave can distinguish the operation host write or read.

	if(irq_status & FLD_HOST_CMD_IRQ){  //both host write & read trigger this status

		i2c_clear_interrupt_status(FLD_HOST_CMD_IRQ|FLD_HOST_READ_IRQ);//clear all irq status

		if(irq_status & FLD_HOST_READ_IRQ){  //host read

			i2c_read_cnt ++;  //for debug
			i2c_irq_flag |= IRQ_IIC_READ;
		}
		else{  //host write

			i2c_write_cnt ++;  //for debug


			i2c_irq_flag |= IRQ_IIC_WRITE;


			/*********** copy the data written by i2c master to slave_rx_buff_debug for debug ****************/

			memcpy( (unsigned char*)(slave_rx_buff_debug + slave_rx_index*DBG_DATA_LEN), (unsigned char*)slave_mapping_buff, DBG_DATA_LEN);


			slave_rx_index ++;
			if(slave_rx_index>=DBG_DATA_NUM){
				slave_rx_index = 0;
			}
			/******************************** end ****************************************************/
		}



	}


}
void i2c_slave_test_init(void)
{


	i2c_gpio_set(I2C_GPIO_GROUP_A3A4);  	//SDA/CK : C0/C1


	//slave device id 0x5C(write) 0x5D(read)
	//i2c slave mapping mode, master no need send any address when reading/writing,
	//for i2c master: writing data buffer is slave_mapping_buff,
	//and reading data buffer is (slave_mapping_buff+64) (this offset 64 is managed by MCU hardware, user can not change it)
	i2c_slave_init(0x5C, I2C_SLAVE_MAP, (unsigned char *)slave_mapping_buff + 64);

	//slave_mapping_buff + 64 is master reading data buffer in mapping mode, put some data here for master read

	memset(&slave_mapping_buff[64],0x55,DBG_DATA_LEN);

	irq_set_mask(FLD_IRQ_MIX_CMD_EN);
	irq_enable();
}


void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	i2c_slave_test_init();



}



_attribute_ram_code_sec_noinline_ int main (void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();
    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1);
    gpio_write(GREEN_LED_PIN, 0);


	while(1)
	{

		WaitMs(50);
		gpio_toggle(GREEN_LED_PIN);

	}

}


















