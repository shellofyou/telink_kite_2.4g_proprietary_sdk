#include "common.h"
#include "driver.h"
#include "../../fw_update/fw_update_phy.h"
#include "../../fw_update/fw_update.h"

#define    GPIO_IRQ_PIN        GPIO_PB4

extern volatile unsigned char FW_UPDATE_MasterTrig;

_attribute_session_(".ram_code") void irq_handler(void)
{
	unsigned char uart_dma_irqsrc= dma_chn_irq_status_get();
	unsigned int irq_src = irq_get_src();
	//1. UART irq
	if(uart_dma_irqsrc & FLD_DMA_CHN_UART_RX)
	{
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
		FW_UPDATE_PHY_RxIrqHandler();
	}
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_TX)
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);

        FW_UPDATE_PHY_TxIrqHandler();
    }

    if (irq_src & FLD_IRQ_GPIO_EN)
    {
    	 if (0 == gpio_read(GPIO_IRQ_PIN))
    	 {
    		 WaitMs(10);
    		 if (0 == gpio_read(GPIO_IRQ_PIN))
    		 {
    			 while(0 == gpio_read(GPIO_IRQ_PIN));
    			 FW_UPDATE_MasterTrig = 1;
    		 }
    	 }
    }
    //irq_clr_src2(FLD_IRQ_GPIO_EN);
    irq_clr_src();
}



