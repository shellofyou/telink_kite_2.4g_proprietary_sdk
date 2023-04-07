#include "driver.h"
#include "common.h"


#define SLAVE_REGADDR1  0x04
#define SLAVE_REGADDR2  0x10
#define SLAVE_REGADDR3  0x20
#define SPI_READ_CMD  	0x80// spi read command
#define SPI_WRITE_CMD 	0x00// spi write command
#define DBG_DATA_LEN    16
#define SPI_BUFFER_CNT  32
#define SPI_DATA_HEAD  0xAA
#define SPI_DATA_END   0xBB
/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	24000000

#define     SPI_CS_PIN				        GPIO_PD6//SPI CS pin
#define     GREEN_LED_PIN                   GPIO_PD3

unsigned char slaveRegAddr_WriteCMD[] = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_WRITE_CMD};
unsigned char slaveRegAddr_ReadCMD[]  = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_READ_CMD};

volatile unsigned char  buff[SPI_BUFFER_CNT] __attribute__((section(".spi_slave_buff")));
unsigned char slave_rxbuf[SPI_BUFFER_CNT];
int slave_rx_length = 0;
int irq_cnt = 0;
int spi_irq_cnt=0;
unsigned int spi_read_cnt = 0;
unsigned int spi_write_cnt = 0;
unsigned char  irq_status = 0;
unsigned char  irq_status1 = 0;


void spi_slave_irq_handler(void)
{
	if(buff[0]==SPI_DATA_HEAD){//Judge whether DataHead is correct.
		slave_rx_length= buff[1];
		if( buff[slave_rx_length-1]==SPI_DATA_END ){//Judge whether DataEnd is correct.
		   spi_write_cnt++;  //
		   for(int i = 0;i<slave_rx_length;i++){
			   slave_rxbuf[i]=buff[i];//Get data that Master SPI writes to Slave.
		   }
		 }
	}
	else{ spi_read_cnt++;}//master read from slave
}


_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	irq_cnt ++;

	unsigned char  irq_status = reg_spi_slave_irq_status;
	//SPI Interrupt means that every WRITE or READ will generate one interrupt(capture CS signal)
	if(irq_status & FLD_SLAVE_SPI_IRQ)
	{
		reg_spi_slave_irq_status = irq_status;
		spi_irq_cnt ++;
		spi_slave_irq_handler();
	}
}



void spi_slave_test_init(void)
{
	spi_slave_init((unsigned char)(CLOCK_SYS_CLOCK_HZ/(2*500000)-1),SPI_MODE0);           //slave mode init

	spi_slave_gpio_set(SPI_GPIO_GROUP_A2A3A4D6);      //slave mode £ºspi pin set

}


void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on
	//SPI:CK/CN/DO/DI   A4/D6/A2/A3, D7/D2/B7/B6

	gpio_set_func(GREEN_LED_PIN ,AS_GPIO);
	gpio_set_output_en(GREEN_LED_PIN, 1); 		//enable output
	gpio_set_input_en(GREEN_LED_PIN ,0);			//disable input
	gpio_write(GREEN_LED_PIN, 0);              	//LED On

	spi_slave_test_init();

	reg_irq_mask |= FLD_IRQ_MIX_CMD_EN;
	irq_enable();
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
		WaitMs(1000);
		gpio_toggle(GREEN_LED_PIN);

	}
}





