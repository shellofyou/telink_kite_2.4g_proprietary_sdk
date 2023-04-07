/********************************************************************************************************
 * @file	ota.h
 *
 * @brief	This is the header file for 8355
 *
 * @author	2.4G Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#ifndef _OTA_H_
#define _OTA_H_




#define OTA_SLAVE_BIN_ADDR_0x40000   0x40000
#define OTA_SLAVE_BIN_ADDR_0x20000   0x20000

#define OTA_SLAVE_BIN_ADDR           OTA_SLAVE_BIN_ADDR_0x20000

#define OTA_FRAME_TYPE_CMD        0x01
#define OTA_FRAME_TYPE_DATA       0x02
#define OTA_FRAME_TYPE_ACK        0x03

#define OTA_CMD_ID_START_REQ      0x01
#define OTA_CMD_ID_START_RSP      0x02
#define OTA_CMD_ID_END_REQ        0x03
#define OTA_CMD_ID_END_RSP        0x04
#define OTA_CMD_ID_VERSION_REQ    0x05
#define OTA_CMD_ID_VERSION_RSP    0x06




#define OTA_FRAME_PAYLOAD_MAX     (48+2)
#define OTA_RETRY_MAX             3
#define OTA_APPEND_INFO_LEN              2 // FW_CRC 2 BYTE

typedef struct {
    unsigned int FlashAddr;
    unsigned int TotalBinSize;
    unsigned short MaxBlockNum;
    unsigned short BlockNum;
    unsigned short PeerAddr;
    unsigned short FwVersion;
    unsigned short FwCRC;
    unsigned short PktCRC;
    unsigned short TargetFwCRC;
    unsigned char State;
    unsigned char RetryTimes;
    unsigned char FinishFlag;
} OTA_CtrlTypeDef;

typedef struct {
    unsigned char Type;
    unsigned char Payload[OTA_FRAME_PAYLOAD_MAX];
} OTA_FrameTypeDef;

enum {
    OTA_MASTER_STATE_IDLE = 0,
    OTA_MASTER_STATE_FW_VER_WAIT,
    OTA_MASTER_STATE_START_RSP_WAIT,
    OTA_MASTER_STATE_DATA_ACK_WAIT,
    OTA_MASTER_STATE_END_RSP_WAIT,
    OTA_MASTER_STATE_END,
    OTA_MASTER_STATE_ERROR,
};

enum {
    OTA_SLAVE_STATE_IDLE = 0,
    OTA_SLAVE_STATE_FW_VERSION_READY,
    OTA_SLAVE_STATE_START_READY,
    OTA_SLAVE_STATE_DATA_READY,
    OTA_SLAVE_STATE_END_READY,
    OTA_SLAVE_STATE_END,
    OTA_SLAVE_STATE_ERROR
};

enum {
    OTA_MSG_TYPE_INVALID_DATA = 0,
    OTA_MSG_TYPE_DATA,
    OTA_MSG_TYPE_TIMEOUT,
};



typedef enum{

	GEN_FSK_STX_MODE = 0,
	GEN_FSK_SRX_MODE = 1,

}Gen_Fsk_Mode_Slect;

extern void OTA_MasterInit(unsigned int OTABinAddr, unsigned short SlaveAddr, unsigned short FwVer);
extern void OTA_MasterStart(void);

extern void OTA_SlaveInit(unsigned int OTABinAddr, unsigned short MasterAddr, unsigned short FwVer);
extern void OTA_SlaveStart(void);

extern void OTA_RxIrq(unsigned char *Data);
extern void OTA_RxTimeoutIrq(unsigned char *Data);


#endif /*_OTA_H_*/
