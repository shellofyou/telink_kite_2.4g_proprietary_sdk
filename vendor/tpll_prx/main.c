#include "driver.h"
#include "tpll.h"


#define PTX_CHANNEL    0

#define    DEBUG_PIN      GPIO_PD3

volatile unsigned char rx_flag = 0;
volatile unsigned char rx_data[128] = {0};


static void User_Init(signed short chn)
{

    WaitMs(3000);

    //rf configuration
    TPLL_SetDatarate(TPLL_DR_2M);
    TPLL_SetOutputPower(TPLL_RF_POWER_0DBM);
    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);
    TPLL_ClosePipe(TPLL_PIPE_ALL);

    unsigned char rx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7};
    TPLL_SetAddress(TPLL_PIPE0,rx_address);
    TPLL_OpenPipe(TPLL_PIPE0,1);




    TPLL_ModeSet(TPLL_MODE_PRX);
    TPLL_SetRFChannel(chn);
    TPLL_TxSettleSet(149);
    TPLL_RxSettleSet(80);
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
    //enable irq
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_TX|FLD_RF_IRQ_TX_DS|FLD_RF_IRQ_RX_DR);
    irq_enable(); //enable general irq

}

static unsigned char ack_payload[32] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
										0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
										0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
										0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
unsigned char ack_payload_length = 32;
void main(void)
{
    unsigned char i;

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_set_output_en(DEBUG_PIN, 1); //enable output
    gpio_set_input_en(DEBUG_PIN, 0); //disable input
    gpio_write(DEBUG_PIN,0);

    User_Init(4);

    //TPLL_WriteAckPayload(TPLL_PIPE0, ack_payload, ack_payload_length);
    TPLL_PRXTrig();

     while (1)
    {

        if(1 == rx_flag)
        {
            rx_flag = 0;
            gpio_toggle(DEBUG_PIN);
            TPLL_ReadRxPayload(rx_data);
            while(!TPLL_TxFifoEmpty(0));
            TPLL_WriteAckPayload(TPLL_PIPE0, ack_payload, ack_payload_length);
        }
    }
}




