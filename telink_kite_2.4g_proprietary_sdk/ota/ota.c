/********************************************************************************************************
 * @file	ota.c
 *
 * @brief	This is the source file for 825x
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

#include "ota.h"
#include "common.h"
#include "driver.h"
#include "mac.h"

#include "genfsk_ll.h"

//#define OTA_MASTER_EN          1
#define MSG_QUEUE_LEN          4
#define OTA_BIN_SIZE_OFFSET    0x18

#define OTA_REBOOT_WAIT        (1000*1000) //in us
#define OTA_BOOT_FLAG_OFFSET   8

typedef struct {
    unsigned int Type;
    unsigned char *Data; //content of msg
} OTA_MsgTypeDef;

typedef struct {
    OTA_MsgTypeDef Msg[MSG_QUEUE_LEN]; //buffer storing msg
    unsigned char Cnt; //current num of msg
    unsigned char ReadPtr; //ptr of first msg should be read next
} OTA_MsgQueueTypeDef;
OTA_MsgQueueTypeDef MsgQueue;

OTA_FrameTypeDef TxFrame;
OTA_FrameTypeDef RxFrame;

static int OTA_MsgQueuePush(const unsigned char *Data, const unsigned int Type, OTA_MsgQueueTypeDef *Queue)
{
    if (Queue->Cnt < MSG_QUEUE_LEN) {
        Queue->Msg[(Queue->ReadPtr + Queue->Cnt) % MSG_QUEUE_LEN].Data = (unsigned char *)Data;
        Queue->Msg[(Queue->ReadPtr + Queue->Cnt) % MSG_QUEUE_LEN].Type = Type;
        Queue->Cnt++;
        return 1;
    }

    return 0;
}

static OTA_MsgTypeDef *OTA_MsgQueuePop(OTA_MsgQueueTypeDef *Queue)
{
    OTA_MsgTypeDef *ret = NULL;

    if (Queue->Cnt > 0) {
        ret = &(Queue->Msg[Queue->ReadPtr]);
        Queue->ReadPtr = (Queue->ReadPtr + 1) % MSG_QUEUE_LEN;
        Queue->Cnt--;
    }

    return ret;
}

static int OTA_BuildCmdFrame(OTA_FrameTypeDef *Frame, const unsigned char CmdId, const unsigned char *Value, unsigned short Len)
{
    Frame->Type = OTA_FRAME_TYPE_CMD;
    Frame->Payload[0] = CmdId;
    if (Value) {
        memcpy(&Frame->Payload[1], Value, Len);
    }
    unsigned int fram_length = Len+2;
    return fram_length;
}

void OTA_RxIrq(unsigned char *Data)
{
    if (NULL == Data) {
        OTA_MsgQueuePush(NULL, OTA_MSG_TYPE_INVALID_DATA, &MsgQueue);
    }
    else {
        if (Data[0]) {
            OTA_MsgQueuePush(Data, OTA_MSG_TYPE_DATA, &MsgQueue);
        }
        else {
            OTA_MsgQueuePush(NULL, OTA_MSG_TYPE_INVALID_DATA, &MsgQueue);
        }
    }
}

void OTA_RxTimeoutIrq(unsigned char *Data)
{
    OTA_MsgQueuePush(NULL, OTA_MSG_TYPE_TIMEOUT, &MsgQueue);
}

static unsigned short OTA_CRC16_Cal(unsigned short crc, unsigned char* pd, int len)
{
    // unsigned short       crc16_poly[2] = { 0, 0xa001 }; //0x8005 <==> 0xa001
    unsigned short      crc16_poly[2] = { 0, 0x8408 }; //0x1021 <==> 0x8408
    //unsigned short        crc16_poly[2] = { 0, 0x0811 }; //0x0811 <==> 0x8810
    //unsigned short        crc = 0xffff;
    int i, j;

    for (j = len; j > 0; j--)
    {
        unsigned char ds = *pd++;
        for (i = 0; i < 8; i++)
        {
            crc = (crc >> 1) ^ crc16_poly[(crc ^ ds) & 1];
            ds = ds >> 1;
        }
    }

    return crc;
}


#ifdef OTA_MASTER_EN

static OTA_CtrlTypeDef MasterCtrl = {0};

static int OTA_IsBlockNumMatch(unsigned char *Payload)
{
    unsigned short BlockNum = Payload[1];
    BlockNum <<= 8;
    BlockNum += Payload[0];

    if (BlockNum == MasterCtrl.BlockNum) {
        return 1;
    }
    else {
        return 0;
    }
}

static int OTA_BuildDataFrame(OTA_FrameTypeDef *Frame)
{
	unsigned int fram_length;
    Frame->Type = OTA_FRAME_TYPE_DATA;
    if ((MasterCtrl.TotalBinSize - MasterCtrl.BlockNum*(OTA_FRAME_PAYLOAD_MAX-2)) > (OTA_FRAME_PAYLOAD_MAX-2)) {
    	fram_length = OTA_FRAME_PAYLOAD_MAX;
    }
    else {
    	fram_length = MasterCtrl.TotalBinSize - MasterCtrl.BlockNum*(OTA_FRAME_PAYLOAD_MAX-2) + 2;
        MasterCtrl.FinishFlag = 1;
    }
    MasterCtrl.BlockNum++;
    memcpy(Frame->Payload, &MasterCtrl.BlockNum, 2);
    flash_read_page(MasterCtrl.FlashAddr,fram_length-2, &Frame->Payload[2]);
    MasterCtrl.FlashAddr += fram_length-2;
    return (1 + fram_length);
}

void OTA_MasterInit(unsigned int OTABinAddr, unsigned short SlaveAddr, unsigned short FwVer)
{
    MasterCtrl.FlashAddr = OTABinAddr;
    //read the size of OTA_bin file
    flash_read_page((unsigned long)MasterCtrl.FlashAddr + OTA_BIN_SIZE_OFFSET, 4, ( unsigned char *)&MasterCtrl.TotalBinSize);
    MasterCtrl.TotalBinSize += OTA_APPEND_INFO_LEN; // APPEND CRC INFO IN BIN TAIL
    MasterCtrl.MaxBlockNum = (MasterCtrl.TotalBinSize + (OTA_FRAME_PAYLOAD_MAX - 2) - 1) / (OTA_FRAME_PAYLOAD_MAX - 2);
    MasterCtrl.BlockNum = 0;
    MasterCtrl.PeerAddr = SlaveAddr;
    MasterCtrl.FwVersion = FwVer;
    MasterCtrl.State = OTA_MASTER_STATE_IDLE;
    MasterCtrl.RetryTimes = 0;
    MasterCtrl.FinishFlag = 0;
}
void OTA_MasterStart(void)
{
    OTA_MsgTypeDef *Msg = OTA_MsgQueuePop(&MsgQueue);
    static int Len = 0;

    if (OTA_MASTER_STATE_IDLE == MasterCtrl.State) {
        Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_VERSION_REQ, 0, 0);
        MAC_SendData(&TxFrame, Len);
        MasterCtrl.State = OTA_MASTER_STATE_FW_VER_WAIT;
    }
    else if (OTA_MASTER_STATE_FW_VER_WAIT == MasterCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
            	Len = (int)Msg->Data[0];
            	RxFrame.Type = Msg->Data[1];
            	memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the valid FW version response
                if ((OTA_FRAME_TYPE_CMD == RxFrame.Type) &&
                (OTA_CMD_ID_VERSION_RSP == RxFrame.Payload[0])) {
                    MasterCtrl.RetryTimes = 0;
                    //compare the received version with that of OTA_bin
                    unsigned short Version = RxFrame.Payload[2];
                    Version <<= 8;
                    Version += RxFrame.Payload[1];

                    if (Version < MasterCtrl.FwVersion) {
                        MasterCtrl.State = OTA_MASTER_STATE_START_RSP_WAIT;
                        Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_START_REQ, &MasterCtrl.MaxBlockNum, sizeof(MasterCtrl.MaxBlockNum));
                        MAC_SendData(&TxFrame, Len);
                    }
                    else {
                        MasterCtrl.State = OTA_MASTER_STATE_ERROR;
                    }
                    return;
                }
            }

            if (MasterCtrl.RetryTimes == OTA_RETRY_MAX) {
                MasterCtrl.State = OTA_MASTER_STATE_ERROR;
                return;
            }
            MasterCtrl.RetryTimes++;
            MAC_SendData(&TxFrame, Len);
        }
    }
    else if (OTA_MASTER_STATE_START_RSP_WAIT == MasterCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
            	Len = (int)Msg->Data[0];
            	RxFrame.Type = Msg->Data[1];
            	memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the valid FW version response
                if ((OTA_FRAME_TYPE_CMD == RxFrame.Type) &&
                (OTA_CMD_ID_START_RSP == RxFrame.Payload[0])) {
                    MasterCtrl.RetryTimes = 0;
                    //read OTA_bin from flash and packet it in OTA data frame
                    MasterCtrl.State = OTA_MASTER_STATE_DATA_ACK_WAIT;
                    Len = OTA_BuildDataFrame(&TxFrame);
                    MAC_SendData(&TxFrame, Len);
                    return;
                }
            }

            if (MasterCtrl.RetryTimes == OTA_RETRY_MAX) {
                MasterCtrl.State = OTA_MASTER_STATE_ERROR;
                return;
            }
            MasterCtrl.RetryTimes++;
            MAC_SendData(&TxFrame, Len);
        }
    }

    else if (OTA_MASTER_STATE_DATA_ACK_WAIT == MasterCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
            	Len = (int)Msg->Data[0];
            	RxFrame.Type = Msg->Data[1];
            	memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the valid OTA data ack
                if ((OTA_FRAME_TYPE_ACK == RxFrame.Type) && OTA_IsBlockNumMatch(RxFrame.Payload)) {
                    MasterCtrl.RetryTimes = 0;
                    if (MasterCtrl.FinishFlag) {
                        MasterCtrl.State = OTA_MASTER_STATE_END_RSP_WAIT;
                        Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_END_REQ, &MasterCtrl.TotalBinSize, sizeof(MasterCtrl.TotalBinSize));
                        MAC_SendData(&TxFrame, Len);
                    }
                    else {
                        //read OTA_bin from flash and packet it in OTA data frame
                        Len = OTA_BuildDataFrame(&TxFrame);
                        MAC_SendData(&TxFrame, Len);
                    }
                    return;
                }
            }

            if (MasterCtrl.RetryTimes == OTA_RETRY_MAX) {
                MasterCtrl.State = OTA_MASTER_STATE_ERROR;
                return;
            }
            MasterCtrl.RetryTimes++;
            MAC_SendData(&TxFrame, Len);
        }
    }

    else if (OTA_MASTER_STATE_END_RSP_WAIT == MasterCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
            	Len = (int)Msg->Data[0];
            	RxFrame.Type = Msg->Data[1];
            	memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the valid FW version response
                if ((OTA_FRAME_TYPE_CMD == RxFrame.Type) &&
                    (OTA_CMD_ID_END_RSP == RxFrame.Payload[0])) {
                    MasterCtrl.RetryTimes = 0;
                    MasterCtrl.State = OTA_MASTER_STATE_END;
                    return;
                }
            }
            if (MasterCtrl.RetryTimes == OTA_RETRY_MAX) {
                MasterCtrl.State = OTA_MASTER_STATE_ERROR;
                return;
            }
            MasterCtrl.RetryTimes++;
            MAC_SendData(&TxFrame, Len);
        }
    }
    else if (OTA_MASTER_STATE_END == MasterCtrl.State) {
        while (1) {
            gpio_set_func(GPIO_PD4, AS_GPIO);
            gpio_set_output_en(GPIO_PD4, 1);
            gpio_write(GPIO_PD4, 1);
            WaitMs(60);
            gpio_write(GPIO_PD4, 0);
            WaitMs(120);
            gpio_write(GPIO_PD4, 1);
            WaitMs(60);
            gpio_write(GPIO_PD4, 0);
            WaitMs(120);
        }
    }
    else if (OTA_MASTER_STATE_ERROR == MasterCtrl.State) {
    	while (1){
    	gpio_set_func(GPIO_PD5, AS_GPIO);
		gpio_set_output_en(GPIO_PD5, 1);
		gpio_write(GPIO_PD5, 1);
		WaitMs(60);
		gpio_write(GPIO_PD5, 0);
		WaitMs(120);
		gpio_write(GPIO_PD5, 1);
		WaitMs(60);
		gpio_write(GPIO_PD5, 0);
		WaitMs(120);
    	}
    }
}

#else /*OTA_MASTER_EN*/
#define OTA_MASTER_LISTENING_DURATION    (5*1000*1000) //in us

static OTA_CtrlTypeDef SlaveCtrl = {0};

static int OTA_BuildAckFrame(OTA_FrameTypeDef *Frame, unsigned short BlockNum)
{
	unsigned int fram_length;
    Frame->Type = OTA_FRAME_TYPE_ACK;
    fram_length = 2;
    Frame->Payload[0] = BlockNum & 0xff;
    Frame->Payload[1] = BlockNum >> 8;


    return (1 + fram_length);
}

static void OTA_FlashErase(void)
{
    int SectorAddr = SlaveCtrl.FlashAddr;
    int i = 0;
    for (i = 0; i < 15; i++) {
        flash_erase_sector(SectorAddr);
        SectorAddr += 0x1000;
    }
}

void OTA_SlaveInit(unsigned int OTABinAddr, unsigned short MasterAddr, unsigned short FwVer)
{
    SlaveCtrl.FlashAddr = OTABinAddr;
    SlaveCtrl.BlockNum = 0;
    SlaveCtrl.PeerAddr = MasterAddr;
    SlaveCtrl.FwVersion = FwVer;
    SlaveCtrl.State = OTA_SLAVE_STATE_IDLE;
    SlaveCtrl.RetryTimes = 0;
    SlaveCtrl.FinishFlag = 0;
    SlaveCtrl.FwCRC = SlaveCtrl.PktCRC = 0;

    //erase the OTA write area
    OTA_FlashErase();
}

void OTA_SlaveStart(void)
{
    OTA_MsgTypeDef *Msg = OTA_MsgQueuePop(&MsgQueue);
    static int Len = 0;
    if (OTA_SLAVE_STATE_IDLE == SlaveCtrl.State) {
        SlaveCtrl.State = OTA_SLAVE_STATE_FW_VERSION_READY;
        MAC_RecvData(OTA_MASTER_LISTENING_DURATION);
    }
    else if (OTA_SLAVE_STATE_FW_VERSION_READY == SlaveCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
            	Len = (int)Msg->Data[0];
            	RxFrame.Type = Msg->Data[1];
            	memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the valid FW version request
                if ((OTA_FRAME_TYPE_CMD == RxFrame.Type) &&
                (OTA_CMD_ID_VERSION_REQ == RxFrame.Payload[0])) {
                    SlaveCtrl.RetryTimes = 0;
                    //send the FW version response to master
                    SlaveCtrl.State = OTA_SLAVE_STATE_START_READY;
                    Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_VERSION_RSP, (unsigned char *)&SlaveCtrl.FwVersion, sizeof(SlaveCtrl.FwVersion));
                    MAC_SendData((unsigned char *)&TxFrame, Len);
                    return;
                }
            }

            if (SlaveCtrl.RetryTimes == OTA_RETRY_MAX) {
                SlaveCtrl.State = OTA_SLAVE_STATE_ERROR;
                return;
            }
            SlaveCtrl.RetryTimes++;
            MAC_RecvData(OTA_MASTER_LISTENING_DURATION);
        }
    }
    else if (OTA_SLAVE_STATE_START_READY == SlaveCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
				Len = (int)Msg->Data[0];
				RxFrame.Type = Msg->Data[1];
				memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                if (OTA_FRAME_TYPE_CMD == RxFrame.Type) {
                    //if receive the FW version request again
                    if (OTA_CMD_ID_VERSION_REQ == RxFrame.Payload[0]) {
                        SlaveCtrl.RetryTimes = 0;
                        //send the FW version response again to master
                        MAC_SendData((unsigned char *)&TxFrame, Len);
                        return;
                    }
                    //if receive the OTA start request
                    if (OTA_CMD_ID_START_REQ == RxFrame.Payload[0]) {

                        SlaveCtrl.RetryTimes = 0;
                        memcpy(&SlaveCtrl.MaxBlockNum, &RxFrame.Payload[1], sizeof(SlaveCtrl.MaxBlockNum));
                        //send the OTA start response to master
                        SlaveCtrl.State = OTA_SLAVE_STATE_DATA_READY;
                        Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_START_RSP, 0, 0);
                        MAC_SendData((unsigned char *)&TxFrame, Len);
                        return;
                    }
                }
            }

            if (SlaveCtrl.RetryTimes == OTA_RETRY_MAX) {
                SlaveCtrl.State = OTA_SLAVE_STATE_ERROR;
                return;
            }
            SlaveCtrl.RetryTimes++;
            MAC_RecvData(OTA_MASTER_LISTENING_DURATION);
        }
    }
    else if (OTA_SLAVE_STATE_DATA_READY == SlaveCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
				Len = (int)Msg->Data[0];
				RxFrame.Type = Msg->Data[1];
				memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the OTA start request again
                if (OTA_FRAME_TYPE_CMD == RxFrame.Type) {
                    if (OTA_CMD_ID_START_REQ == RxFrame.Payload[0]) {
                        SlaveCtrl.RetryTimes = 0;
                        //send the OTA start response again to master
                        MAC_SendData((unsigned char *)&TxFrame, Len);//need change
                        return;
                    }
                }
                //if receive the OTA data frame
                if (OTA_FRAME_TYPE_DATA == RxFrame.Type) {
                    //check the block number included in the incoming frame
                    unsigned short BlockNum = RxFrame.Payload[1];
                    BlockNum <<= 8;
                    BlockNum += RxFrame.Payload[0];
                    //if receive the same OTA data frame again, just respond with the same ACK
                    if (BlockNum == SlaveCtrl.BlockNum) {
                        SlaveCtrl.RetryTimes = 0;
                        //send the OTA data ack again to master
                        MAC_SendData((unsigned char *)&TxFrame, Len);
                        return;
                    }
                    //if receive the next OTA data frame, just respond with an ACK
                    if (BlockNum == SlaveCtrl.BlockNum + 1) {
                        SlaveCtrl.RetryTimes = 0;
//                        printf("block_num:%d, len:%d, PktCRC:%2x\n", BlockNum, Len - 3, SlaveCtrl.PktCRC);
					   /*
						* write received data to flash,
						* and avoid first block data writing boot flag as head of time.
						*/
					   if(1 == BlockNum)
					   {
						   // unfill boot flag in ota procedure
						   flash_write_page(SlaveCtrl.FlashAddr, 8, &RxFrame.Payload[2]);
						   flash_write_page(SlaveCtrl.FlashAddr + 12, Len - 3 - 12, &RxFrame.Payload[2 + 12]);
					   }
					   else
					   {
						   //write received data to flash
						   flash_write_page(SlaveCtrl.FlashAddr + SlaveCtrl.TotalBinSize, Len - 3, &RxFrame.Payload[2]);
					   }

					   SlaveCtrl.BlockNum = BlockNum;
					   SlaveCtrl.TotalBinSize += (Len - 3);

					   if (SlaveCtrl.MaxBlockNum == BlockNum) {
						SlaveCtrl.PktCRC = OTA_CRC16_Cal(SlaveCtrl.PktCRC, &RxFrame.Payload[2], Len - 3 - OTA_APPEND_INFO_LEN);
						   SlaveCtrl.State = OTA_SLAVE_STATE_END_READY;
					   }
					   else
					   {
						SlaveCtrl.PktCRC = OTA_CRC16_Cal(SlaveCtrl.PktCRC, &RxFrame.Payload[2], Len - 3);
					   }
					   //send the OTA data ack to master
                        unsigned int Length = OTA_BuildAckFrame(&TxFrame, BlockNum);
                        MAC_SendData((unsigned char *)&TxFrame, Length);
                        return;
                    }
                }
            }

            if (SlaveCtrl.RetryTimes == OTA_RETRY_MAX) {
                SlaveCtrl.State = OTA_SLAVE_STATE_ERROR;
                return;
            }
            SlaveCtrl.RetryTimes++;
            MAC_RecvData(OTA_MASTER_LISTENING_DURATION);
        }
    }
    else if (OTA_SLAVE_STATE_END_READY == SlaveCtrl.State) {
        if (Msg) {
            //if receive a valid rf packet
            if (Msg->Type == OTA_MSG_TYPE_DATA) {
				Len = (int)Msg->Data[0];
				RxFrame.Type = Msg->Data[1];
				memcpy(RxFrame.Payload, Msg->Data+2, Len-1);
                //if receive the last OTA data frame again
                if (OTA_FRAME_TYPE_DATA == RxFrame.Type) {
                    //check the block number included in the incoming frame
                    unsigned short BlockNum = RxFrame.Payload[1];
                    BlockNum <<= 8;
                    BlockNum += RxFrame.Payload[0];
                    //if receive the same OTA data frame again, just respond with the same ACK
                    if (BlockNum == SlaveCtrl.BlockNum) {
                        SlaveCtrl.RetryTimes = 0;
                        //send the OTA data ack again to master
                        MAC_SendData((unsigned char *)&TxFrame, Len);
                        return;
                    }
                }
                //if receive the OTA end request
                if (OTA_FRAME_TYPE_CMD == RxFrame.Type) {
                    if (OTA_CMD_ID_END_REQ == RxFrame.Payload[0]) {
                        SlaveCtrl.RetryTimes = 0;
                        unsigned int BinSize = 0;
                        memcpy(&BinSize, &RxFrame.Payload[1], sizeof(BinSize));
                        if (SlaveCtrl.TotalBinSize != BinSize) {
                            SlaveCtrl.State = OTA_SLAVE_STATE_ERROR;
                            return;
                        }
                        SlaveCtrl.State = OTA_SLAVE_STATE_END;
                        //send the OTA end response to master
                        Len = OTA_BuildCmdFrame(&TxFrame, OTA_CMD_ID_END_RSP, 0, 0);
                        MAC_SendData((unsigned char *)&TxFrame, Len);
                        WaitMs(5); //wait for transmission finished
                        return;
                    }
                }
            }

            if (SlaveCtrl.RetryTimes == OTA_RETRY_MAX) {
                SlaveCtrl.State = OTA_SLAVE_STATE_ERROR;
                return;
            }
            SlaveCtrl.RetryTimes++;
            MAC_RecvData(OTA_MASTER_LISTENING_DURATION);
        }
    }
    else if (OTA_SLAVE_STATE_END == SlaveCtrl.State) {
#if 1
        // 1. todo FW crc check
//        int max_block_num = (SlaveCtrl.TotalBinSize + OTA_FRAME_PAYLOAD_MAX -2 - 1) / (OTA_FRAME_PAYLOAD_MAX - 2);
        unsigned char bin_buf[48] = {0};
        int block_idx = 0;
        int len = 0;
        flash_read_page((unsigned long)SlaveCtrl.FlashAddr + SlaveCtrl.TotalBinSize - OTA_APPEND_INFO_LEN,
                2, &SlaveCtrl.TargetFwCRC);
        while (1)
        {
            if (SlaveCtrl.TotalBinSize - block_idx * (OTA_FRAME_PAYLOAD_MAX - 2) > (OTA_FRAME_PAYLOAD_MAX - 2))
            {
                len = OTA_FRAME_PAYLOAD_MAX - 2;
                flash_read_page((unsigned long)SlaveCtrl.FlashAddr + block_idx * (OTA_FRAME_PAYLOAD_MAX - 2),
                        len, &bin_buf[0]);
                if (0 == block_idx)
                {
                    // fill the boot flag mannually
                    bin_buf[8] = 0x4b;
                    bin_buf[9] = 0x4e;
                    bin_buf[10] = 0x4c;
                    bin_buf[11] = 0x54;
                }
                SlaveCtrl.FwCRC = OTA_CRC16_Cal(SlaveCtrl.FwCRC, &bin_buf[0], len);
            }
            else
            {
                len = SlaveCtrl.TotalBinSize - (block_idx * (OTA_FRAME_PAYLOAD_MAX - 2))- OTA_APPEND_INFO_LEN;
                flash_read_page((unsigned long)SlaveCtrl.FlashAddr + block_idx * (OTA_FRAME_PAYLOAD_MAX - 2),
                        len, &bin_buf[0]);
                SlaveCtrl.FwCRC = OTA_CRC16_Cal(SlaveCtrl.FwCRC, &bin_buf[0], len);
                break;
            }
            block_idx++;
//            printf("fw block idx:%d, len:%d, FwCRC:%2x\r\n", block_idx, len, SlaveCtrl.FwCRC);
        }
//        printf("fw block idx:%d, len:%d, FwCRC:%2x  \r\n", block_idx + 1, len, SlaveCtrl.FwCRC);
//        printf("pkt_crc:%2x, fw_crc:%2x, target_fw_crc:%2x\r\n", SlaveCtrl.PktCRC, SlaveCtrl.FwCRC, SlaveCtrl.TargetFwCRC);
        if (SlaveCtrl.FwCRC != SlaveCtrl.PktCRC || SlaveCtrl.TargetFwCRC != SlaveCtrl.FwCRC)
        {
//            printf("Crc Check Error\r\n");
            SlaveCtrl.State = OTA_MASTER_STATE_ERROR;
            return;
        }
#endif
        // 2. set next boot flag
        unsigned int utmp = 0x544C4E4B;
        flash_write_page(SlaveCtrl.FlashAddr + 8, 4, (unsigned char *)&utmp);
        //clear current boot flag
        unsigned char tmp = 0x00;
#if (OTA_SLAVE_BIN_ADDR == OTA_SLAVE_BIN_ADDR_0x20000)
        flash_write_page(SlaveCtrl.FlashAddr ? 0x08 : 0x20008, 1, &tmp);
#else
		flash_write_page(0x20008, 1, &tmp);
		flash_write_page(SlaveCtrl.FlashAddr ? 0x08 : 0x40008, 1, &tmp);
#endif
        if (SlaveCtrl.FlashAddr == 0x00)
        {
//            printf("cur_boot_addr:%4x, next_boot_addr:%4x\r\n", OTA_SLAVE_BIN_ADDR, 0x0000);

        }
        else if (SlaveCtrl.FlashAddr == OTA_SLAVE_BIN_ADDR_0x20000)
        {
//        	printf("cur_boot_addr:%4x, next_boot_addr:%4x\r\n", 0x0000, OTA_SLAVE_BIN_ADDR_0x20000);
        }
        else
        {
//        	printf("cur_boot_addr:%4x, next_boot_addr:%4x\r\n", 0x0000, OTA_SLAVE_BIN_ADDR_0x40000);
        }
        //reboot
        irq_disable();
        WaitMs(1000);
        start_reboot();
        while(1);
    }
    else if (OTA_SLAVE_STATE_ERROR == SlaveCtrl.State) {
        //erase the OTA write area
        OTA_FlashErase();
        irq_disable();
//        cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, ClockTime() + OTA_REBOOT_WAIT * 16);
        while (1) {
        	gpio_set_func(GPIO_PD5, AS_GPIO);
			gpio_set_output_en(GPIO_PD5, 1);
			gpio_write(GPIO_PD5, 1);
			WaitMs(60);
			gpio_write(GPIO_PD5, 0);
			WaitMs(120);
			gpio_write(GPIO_PD5, 1);
			WaitMs(60);
			gpio_write(GPIO_PD5, 0);
			WaitMs(120);
        }
    }
}

#endif /*OTA_MASTER_EN*/
















