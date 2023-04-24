#include "fw_update_phy.h"
#include "driver.h"
#include "common.h"

#define PHY_TX_BUF_LEN                96
#define PHY_RX_BUF_LEN                96
#define PHY_RX_BUF_NUM                3

#define UART_DATA_LEN    (96-4)      //data max ?    (UART_DATA_LEN+4) must 16 byte aligned
typedef struct{
    unsigned int dma_len;        // dma len must be 4 byte
    unsigned char data[UART_DATA_LEN];
}uart_data_t;




PHY_Cb_t PHYRxCb = NULL;
static volatile unsigned char PHY_TxFinished = 0;

static uart_data_t PHY_TxBuf __attribute__ ((aligned (4))) = {};


static uart_data_t PHY_RxBuf[PHY_RX_BUF_NUM] __attribute__ ((aligned (4))) = {};
static unsigned char PHY_RxPtr = 0;



void FW_UPDATE_PHY_Init(const PHY_Cb_t RxCb)
{
    //Set UART Rx irq callback
    PHYRxCb = RxCb;

    //config UART module
    uart_recbuff_init(  (unsigned short *)&PHY_RxBuf[PHY_RxPtr], PHY_RX_BUF_LEN);

    uart_gpio_set(UART_TX_PA2, UART_RX_PD6);// uart tx/rx pin set
    uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

    uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);	//115200

    uart_dma_enable(1, 1);     //uart data in hardware buffer moved by dma, so we need enable them first

	irq_set_mask(FLD_IRQ_DMA_EN);

	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);       //uart Rx/Tx dma irq enable

	uart_irq_enable(0, 0);      //uart Rx/Tx irq no need, disable them

}

int FW_UPDATE_PHY_SendData(const unsigned char *Payload, const int PayloadLen)
{

    //set UART DMA length
	PHY_TxBuf.dma_len = PayloadLen;

    //fill the contents of UART transmission
    memcpy(&PHY_TxBuf.data, Payload, PayloadLen);

    uart_dma_send((unsigned short*)&PHY_TxBuf);

    while(!PHY_TxFinished);
    PHY_TxFinished = 0;

    return PayloadLen;
}

void FW_UPDATE_PHY_RxIrqHandler(void)
{
    //set next rx_buf
	unsigned char *RxPacket = PHY_RxBuf[PHY_RxPtr].data;
	PHY_RxPtr = (PHY_RxPtr + 1) % PHY_RX_BUF_NUM;
	uart_recbuff_init(  (unsigned short *)&PHY_RxBuf[PHY_RxPtr], PHY_RX_BUF_LEN);
    if (PHYRxCb) {
        PHYRxCb(&RxPacket[0]);
    }
}

void FW_UPDATE_PHY_TxIrqHandler(void)
{
    PHY_TxFinished = 1;
}
