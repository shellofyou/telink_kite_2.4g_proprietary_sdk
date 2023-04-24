/********************************************************************************************************
 * @file     gui.h
 *
 * @brief    This file provides set of functions for Telink ESL Node device
 *
 * @author   jian.zhang@telink-semi.com
 * @date     Jul. 13, 2017
 *
 * @par      Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *           The information contained herein is confidential property of Telink 
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *           Co., Ltd. and the licensee or the terms described here-in. This heading 
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this 
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#ifndef  _TELINK_ESL_GUI_H
#define  _TELINK_ESL_GUI_H

#define GUI_FONT_WIDTH     8
#define GUI_FONT_HEIGHT    16

typedef struct {
int X_channel;
int Y_channel;
int X_MAX;
int Y_MAX;
} NODESIZE_Typedef;

extern NODESIZE_Typedef Node_Size;

extern void GUI_BytesToHexStr(const unsigned char *bytes, int len, unsigned char *str);
extern void GUI_IntToDecStr(unsigned int data, unsigned char *str);
extern unsigned char GUI_DispChar(unsigned char *image, int x, int y, unsigned char data);
extern unsigned char GUI_DispStr(unsigned char *image, int x, int y, const unsigned char *str, unsigned char dir); //dir: 1-horizontal 0-vertical
extern unsigned char GUI_DispPic(unsigned char *image, int x, int y, const unsigned char *pic, unsigned char width, unsigned char height);
extern void GUI_Clear(unsigned char *image, unsigned char colour); //colour: 0-black, 1-white



#endif
