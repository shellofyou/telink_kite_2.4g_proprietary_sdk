#include "driver.h"
#include "common.h"

#define  CLOCK_SYS_CLOCK_HZ 	24000000	//24M

#define rec_buff_Len    16
#define trans_buff_Len    16
volatile unsigned int tx_cnt=0;
volatile unsigned char uart_rx_flag=0;

volatile unsigned int  uart_ndmairq_cnt=0;
volatile unsigned char uart_ndmairq_index=0;

#define rec_buff_Len    16
#define trans_buff_Len  16

__attribute__((aligned(4))) unsigned char rec_buff[rec_buff_Len]={0};
__attribute__((aligned(4))) unsigned char trans_buff[trans_buff_Len] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, \
                                                                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00};



volatile unsigned char parity_error_flag = 0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{


    static unsigned char uart_ndma_irqsrc;
    uart_ndma_irqsrc = uart_ndmairq_get();  ///get the status of uart irq.
    if(uart_ndma_irqsrc){
   	 if(reg_uart_status1&FLD_UART_RX_BUF_IRQ)
   	 {
    //cycle the four registers 0x90 0x91 0x92 0x93,in addition reading will clear the irq.
        if(uart_rx_flag==0){
            rec_buff[uart_ndmairq_cnt++] = reg_uart_data_buf(uart_ndmairq_index);
            uart_ndmairq_index++;
            uart_ndmairq_index &= 0x03;// cycle the four registers 0x90 0x91 0x92 0x93, it must be done like this for the design of SOC.
            if(uart_ndmairq_cnt%trans_buff_Len==0&&uart_ndmairq_cnt!=0){
                uart_rx_flag=1;
            }
        }
        else{
        	 unsigned char uart_fifo_cnt =0;
			 uart_fifo_cnt= reg_uart_buf_cnt&FLD_UART_RX_BUF_CNT;
			 if(uart_fifo_cnt !=0)
			 {
				 for(unsigned char j=0;j<uart_fifo_cnt;j++){
					 uart_ndma_read_byte();
				 }
			 }
        }
    }
 	if(uart_is_parity_error()){			//when stop bit error or parity error.
 		uart_reset();//clear hardware pointer,it will clear error_interrupt as well.
 		uart_ndma_clear_rx_index();
		uart_ndmairq_cnt=0;
		uart_rx_flag=0;
		for(int i=0;i<rec_buff_Len;i++)
		{
			rec_buff[i]=0;
		}
		}

    }
}



void user_init()
{
    WaitMs(2000);  //leave enough time for SWS_reset when power on

    //note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
   // uart_recbuff_init( (unsigned short *)&rec_buff, sizeof(rec_buff));

    uart_gpio_set(UART_TX_PA2, UART_RX_PB0);// uart tx/rx pin set

    uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

    //baud rate: 115200
    uart_init_baudrate(115200,CLOCK_SYS_CLOCK_HZ,PARITY_NONE, STOP_BIT_ONE);

    uart_dma_enable(0, 0);

    irq_disable_type(FLD_IRQ_DMA_EN);

    dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 0);

    uart_irq_enable(1,0);   //uart RX irq enable

    uart_ndma_irq_triglevel(1,0);   //set the trig level. 1 indicate one byte will occur interrupt

    uart_mask_error_irq_enable();// open uart_error_mask,when stop bit error or parity error,it will enter error_interrupt.

    irq_enable();
}


_attribute_ram_code_sec_noinline_ int main (void)
{
    cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_init(1);

    user_init();



    while (1) {

    	WaitMs(1000);
        if(uart_rx_flag > 0){
            uart_ndmairq_cnt=0; //Clear uart_ndmairq_cnt
            uart_rx_flag=0;
            trans_buff[0] ++;
        }
			for(unsigned char i=0;i<trans_buff_Len;i++){
				uart_ndma_send_byte(trans_buff[i]);

        }
    }
}

