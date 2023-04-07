#include "driver.h"
#include "common.h"



#define 	DBG_DATA_LEN					16
#define 	DBG_DATA_NUM					16
#define 	SLAVE_DMA_MODE_ADDR_WRITE		0x4A000  //i2c master write data to  0x4A000
#define 	SLAVE_DMA_MODE_ADDR_READ		0x4A000  //i2c master read data from 0x4A000

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  		24000000
#define I2C_CLK_SPEED				200000
//write buff
volatile unsigned char i2c_master_tx_buff[DBG_DATA_LEN] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//read buff
volatile unsigned char i2c_master_rx_buff[DBG_DATA_LEN] = {0};
unsigned char master_rx_buff_debug[DBG_DATA_LEN *DBG_DATA_NUM];   //store i2c master reading data here to debug
unsigned int master_rx_index = 0;



void i2c_master_test_init(void)
{

	//I2C pin set

	i2c_gpio_set(I2C_GPIO_GROUP_A3A4);  	//SDA/CK : C0/C1




	//slave device id 0x5C(write) 0x5D(read)
	//i2c clock 200K, only master need set i2c clock
	i2c_master_init(0x5c, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*I2C_CLK_SPEED)) );

}
void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	i2c_master_test_init();

}


_attribute_ram_code_sec_noinline_ int main (void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();


	while(1){


		i2c_master_tx_buff[0] += 1;
		//8258 slave dma mode, sram address(0x40000~0x4FFFF) length should be 3 byte
		i2c_write_series(SLAVE_DMA_MODE_ADDR_WRITE, 3, (unsigned char *)i2c_master_tx_buff, DBG_DATA_LEN);

		i2c_read_series(SLAVE_DMA_MODE_ADDR_READ,  3, (unsigned char *)i2c_master_rx_buff, DBG_DATA_LEN);

		/*********** copy the data read by i2c master from slave for debug  ****************/
		memcpy( (unsigned char *)(master_rx_buff_debug + master_rx_index*DBG_DATA_LEN), (unsigned char *)i2c_master_rx_buff, DBG_DATA_LEN);

		master_rx_index ++;
		if(master_rx_index >= DBG_DATA_NUM){
			master_rx_index = 0;
		}

	}


}




