#include "driver.h"
#include "tpll.h"


#define PTX_CHANNEL    0

#define Debug_pin GPIO_PD3

static volatile unsigned char ack_payload[64] = {};
static volatile unsigned char tx_data[32] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                                             0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
                                             0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
                                             0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20};
volatile unsigned char rx_flag = 0;
volatile unsigned char ds_flag = 0;
volatile unsigned char maxretry_flag = 0;

static void User_Init(signed short chn)
{

    gpio_set_output_en(Debug_pin, 1); //enable output
    gpio_set_input_en(Debug_pin, 0); //disable input
    gpio_write(Debug_pin, 1); //LED Off

    //rf configuration
    TPLL_SetDatarate(TPLL_DR_2M);
    TPLL_SetOutputPower(TPLL_RF_POWER_0DBM);
    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);
    TPLL_ClosePipe(TPLL_PIPE_ALL);

#if PTX_CHANNEL == 0
    //unsigned char tx_address[3] = {0xe7,0xe7,0xe7};
    unsigned char tx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7}; //{0xaa,0xbb,0xcc,0xdd,0xee};
    TPLL_SetAddress(TPLL_PIPE0, tx_address);
    TPLL_OpenPipe(TPLL_PIPE0, 1);
    TPLL_SetTXPipe(TPLL_PIPE0);
#endif

#if PTX_CHANNEL == 1
    unsigned char tx_address[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address);
    TPLL_OpenPipe(TPLL_PIPE1, 1);
    TPLL_SetTXPipe(TPLL_PIPE1);
#endif

#if PTX_CHANNEL == 2
    unsigned char tx_address[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address);
    tx_address[0] = 0x22;
    TPLL_SetAddress(TPLL_PIPE2, &tx_address[0]);
    TPLL_OpenPipe(TPLL_PIPE2, 1);
    TPLL_SetTXPipe(TPLL_PIPE2);
#endif

#if PTX_CHANNEL == 3
    unsigned char tx_address[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address);
    tx_address[0] = 0x33;
    TPLL_SetAddress(TPLL_PIPE3, &tx_address[0]);
    TPLL_OpenPipe(TPLL_PIPE3, 1);
    TPLL_SetTXPipe(TPLL_PIPE3);
#endif


    TPLL_ModeSet(TPLL_MODE_PTX);
    TPLL_SetRFChannel(chn);
    TPLL_SetAutoRetry(0,150);  //5,150
    TPLL_RxTimeoutSet(500);
    TPLL_RxSettleSet(80);
    TPLL_TxSettleSet(149);


    WaitUs(150);
    //configure irq
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);

    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_TX|FLD_RF_IRQ_TX_DS|FLD_RF_IRQ_RETRY_HIT|FLD_RF_IRQ_RX_DR);
    irq_enable(); //enable general irq

}


int main(void)
{

    unsigned char tmp = 0;
    unsigned char tx_len = 32;

	cpu_wakeup_init();
	user_read_flash_value_calib();
    clock_init(SYS_CLK_24M_Crystal);
    User_Init(4);

    tmp = TPLL_WriteTxPayload(PTX_CHANNEL, tx_data, tx_len);
    TPLL_PTXTrig();

    while(1)
    {
        if (ds_flag ||maxretry_flag)
        {
        	if(ds_flag)
        		gpio_toggle(Debug_pin);
            ds_flag = 0;
            maxretry_flag = 0;
            WaitMs(500);

            tmp = TPLL_WriteTxPayload(PTX_CHANNEL, tx_data, tx_len);
            if(tmp)
            {
                TPLL_PTXTrig();
            }
        }
    }
}
