#ifndef _FW_UPDATE_PHY_H_
#define _FW_UPDATE_PHY_H_

typedef void (*PHY_Cb_t)(unsigned char *Data);

extern void FW_UPDATE_PHY_Init(const PHY_Cb_t RxCb);

extern int FW_UPDATE_PHY_SendData(const unsigned char *Payload, const int PayloadLen);

extern void FW_UPDATE_PHY_RxIrqHandler(void);
extern void FW_UPDATE_PHY_TxIrqHandler(void);

#endif /*_FW_UPDATE_PHY_H_*/
