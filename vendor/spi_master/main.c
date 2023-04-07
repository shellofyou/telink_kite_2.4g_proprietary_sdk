#include "driver.h"
#include "common.h"



#define SLAVE_REGADDR1  0x04
#define SLAVE_REGADDR2  0x10
#define SLAVE_REGADDR3  0x20
#define SPI_READ_CMD  	0x80// spi read command
#define SPI_WRITE_CMD 	0x00// spi write command
#define DBG_DATA_LEN    16

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	24000000

#define     SPI_CS_PIN				        GPIO_PD6//SPI CS pin
#define     GREEN_LED_PIN                   GPIO_PD3

unsigned char slaveRegAddr_WriteCMD[] = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_WRITE_CMD};
unsigned char slaveRegAddr_ReadCMD[]  = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_READ_CMD};

//write buff
volatile unsigned char spi_master_tx_buff[DBG_DATA_LEN]={0xAA,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xBB};
//read buff
volatile unsigned char spi_master_rx_buff[DBG_DATA_LEN]={0x00};

void spi_master_test_init(void)
{
	//spi clock 500K, only master need set i2c clock
	spi_master_init((unsigned char)(CLOCK_SYS_CLOCK_HZ/(2*500000)-1),SPI_MODE0);          //div_clock. spi_clk = sys_clk/((div_clk+1)*2),mode select

	spi_master_gpio_set(SPI_GPIO_GROUP_A2A3A4D6);    //master mode £ºspi pin set

}

void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	gpio_set_func(GREEN_LED_PIN ,AS_GPIO);
	gpio_set_output_en(GREEN_LED_PIN, 1); 		//enable output
	gpio_set_input_en(GREEN_LED_PIN ,0);			//disable input
	gpio_write(GREEN_LED_PIN, 0);              	//LED On

	spi_master_test_init();


}


_attribute_ram_code_sec_noinline_ int main (void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();

	irq_enable();

	while (1) {

		WaitMs(1000);   //1S
		gpio_toggle(GREEN_LED_PIN);
		spi_master_tx_buff[1] ++;

	    spi_write(slaveRegAddr_WriteCMD, 4,(unsigned char*)spi_master_tx_buff, DBG_DATA_LEN,SPI_CS_PIN);// pls refer to the datasheet for the write and read format of spi.

		spi_read( slaveRegAddr_ReadCMD , 4,(unsigned char*)spi_master_rx_buff,DBG_DATA_LEN,SPI_CS_PIN);
	}
}





