#include "driver.h"
#include "common.h"

#define    GREEN_LED_PIN        GPIO_PD3

#define 	DBG_DATA_LEN					16
#define 	DBG_DATA_NUM					16
#define 	SLAVE_DMA_MODE_ADDR_WRITE		0x4A000  //i2c master write data to  0x4A000
#define 	SLAVE_DMA_MODE_ADDR_READ		0x4A000  //i2c master read data from 0x4A000
//dma mode write & read  buff
unsigned char * pBuf_slave_dma_for_write;
unsigned char * pBuf_slave_dma_for_read;
unsigned char slave_rx_buff_debug[DBG_DATA_LEN *DBG_DATA_NUM];   //store i2c master writing data here to debug
unsigned int slave_rx_index = 0;


int i2c_read_cnt = 0;   //for debug
int i2c_write_cnt = 0;  //for debug

#define IRQ_IIC_CLEAR       0
#define IRQ_IIC_READ  		BIT(0)  //read
#define IRQ_IIC_WRITE		BIT(1)  //write
unsigned int i2c_irq_flag = IRQ_IIC_CLEAR;





_attribute_ram_code_sec_noinline_ void irq_handler(void)
{

	unsigned char  irq_status = i2c_get_interrupt_status(FLD_HOST_CMD_IRQ|FLD_HOST_READ_IRQ);;//i2c slave can distinguish the operation host write or read.
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
			memcpy( (unsigned char*)(slave_rx_buff_debug + slave_rx_index*DBG_DATA_LEN), pBuf_slave_dma_for_write, DBG_DATA_LEN);

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
	//i2c slave dma mode,  master need send 3 byte sram address(0x40000~0x4ffff) on i2c bus in i2c reading/writing
	i2c_slave_init(0x5C, I2C_SLAVE_DMA, NULL);

	//write&read sram address in dma mode

	reg_irq_mask |= FLD_IRQ_MIX_CMD_EN;   //enable i2c irq
	irq_enable(); //enable system irq

}

void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	i2c_slave_test_init();

	pBuf_slave_dma_for_write = (unsigned char*)(REG_BASE_ADDR+SLAVE_DMA_MODE_ADDR_WRITE);
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



	while(1){

        WaitMs(50);
        gpio_toggle(GREEN_LED_PIN);

	}

}













