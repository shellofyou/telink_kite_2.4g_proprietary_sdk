/********************************************************************************************************
 * @file     epd.h
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
#ifndef _EPD_H_
#define _EPD_H_
#define FLASH_LOCATION_NODE_IMAGE    0x01e000
#define FLASH_SECTOR_SIZE            0x1000
#define FLASH_PAGE_SIZE              256

enum {
    EPD_SIZE_2_1 = 0,
    EPD_SIZE_2_9,
    EPD_SIZE_4_2,
    EPD_SIZE_NUM,
};

extern const unsigned int epd_size_pixels[EPD_SIZE_NUM];

extern void EPD_Init(void);
extern void EPD_Display(unsigned char *image, int size);
extern void EPD_Close(void);

#endif /*_EPD_H_*/
