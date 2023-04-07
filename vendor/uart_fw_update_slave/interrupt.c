#include "common.h"
#include "driver.h"

#define    GPIO_IRQ_PIN        GPIO_PB4

extern volatile unsigned char FW_UPDATE_SlaveTrig;
volatile unsigned int tx_cnt = 0;
volatile unsigned int rx_cnt = 0;
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os")))   void irq_handler(void)
{
	unsigned char uart_dma_irqsrc= dma_chn_irq_status_get();
	unsigned int irq_src = irq_get_src();
	//gpio irq
    if (irq_src & FLD_IRQ_GPIO_EN)
    {
    	 if (0 == gpio_read(GPIO_IRQ_PIN))
    	 {
    		 WaitMs(10);
    		 if (0 == gpio_read(GPIO_IRQ_PIN))
    		 {
    			 while(0 == gpio_read(GPIO_IRQ_PIN));
    			 FW_UPDATE_SlaveTrig = 1;
    		 }
    	 }
    }
    //uart irq
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_RX)
    	{
    		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
    		FW_UPDATE_PHY_RxIrqHandler();
    		rx_cnt ++;
    	}
        if(uart_dma_irqsrc & FLD_DMA_CHN_UART_TX)
        {
            dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
            tx_cnt ++;
            FW_UPDATE_PHY_TxIrqHandler();
        }

        irq_clr_src();

}
