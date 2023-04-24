#include "driver.h"

volatile unsigned int rx_irq_cnt_rx_dr = 0;
volatile unsigned int rx_irq_cnt_invalid_pid = 0;
volatile unsigned int rx_irq_cnt_rx = 0;
volatile unsigned int rx_irq_cnt_tx = 0;
volatile unsigned int rx_irq_cnt_tx_ds = 0;

extern volatile unsigned char rx_flag;
extern volatile unsigned char rx_data[];

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os"))) void irq_handler(void)
{
    unsigned int src = irq_get_src();
    unsigned short src_rf = rf_irq_src_get();
    unsigned short ret = 0;

    if (src_rf & FLD_RF_IRQ_RX_DR) {
        rf_irq_clr_src(FLD_RF_IRQ_RX_DR);
        rx_irq_cnt_rx_dr++;
        rx_flag = 1;
    }
    if (src_rf & FLD_RF_IRQ_INVALID_PID) {
        rf_irq_clr_src(FLD_RF_IRQ_INVALID_PID);
        rx_irq_cnt_invalid_pid++;
    }
    if (src_rf & FLD_RF_IRQ_TX) {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        rx_irq_cnt_tx++;
    }
    if (src_rf & FLD_RF_IRQ_TX_DS) {
        rf_irq_clr_src(FLD_RF_IRQ_TX_DS);
        rx_irq_cnt_tx_ds++;
    }

    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
}



