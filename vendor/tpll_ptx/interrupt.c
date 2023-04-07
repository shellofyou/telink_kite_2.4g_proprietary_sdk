#include "driver.h"




volatile unsigned int tx_irq_cnt_tx = 0;
volatile unsigned int tx_irq_cnt_invalid_pid = 0;
volatile unsigned int tx_irq_cnt_max_retry = 0;
volatile unsigned int tx_irq_cnt_tx_ds = 0;
volatile unsigned int tx_irq_cnt_rx_dr = 0;

extern volatile unsigned char rx_flag,ds_flag,maxretry_flag;

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler(void)
{
    unsigned int  src = irq_get_src();
    unsigned short src_rf = rf_irq_src_get();
    unsigned char pipe = TPLL_GetTXPipe();

    if (src_rf & FLD_RF_IRQ_TX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        tx_irq_cnt_tx++;

    }
    if (src_rf & FLD_RF_IRQ_INVALID_PID)
    {
        rf_irq_clr_src(FLD_RF_IRQ_INVALID_PID);
        tx_irq_cnt_invalid_pid++;
    }
    if (src_rf & FLD_RF_IRQ_RETRY_HIT)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RETRY_HIT);
        tx_irq_cnt_max_retry++;
        maxretry_flag = 1;
        //adjust rptr
        TPLL_UpdateTXFifoRptr(pipe);

    }
    if (src_rf & FLD_RF_IRQ_TX_DS)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX_DS);
        tx_irq_cnt_tx_ds++;
        ds_flag = 1;

    }
    if (src_rf & FLD_RF_IRQ_RX_DR)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX_DR);
        tx_irq_cnt_rx_dr++;
        rx_flag = 1;
    }
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);


}




