/********************************************************************************************************
 * @file     main.c
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

#include "driver.h"
#include "epd.h"
#include "fonts.h"
#include "gui.h"

static void user_init(void)
{


}
unsigned char data_buf[4736];



void main(void)
{
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);
    user_init();

    while (1) {

    	GUI_Clear(data_buf, 1);
        GUI_DispStr(data_buf, 6, 2, " ESL DEMO", 1);
        GUI_DispPic(data_buf, 220, 0, telink_log, 48, 128);
        GUI_DispStr(data_buf, 6, 4, "IEEE ADDR", 1);
        GUI_DispStr(data_buf, 6, 6, "0X", 1);
        unsigned char prompt_str[20];
        unsigned char ieee_addr[]={0,0,0,0};
        GUI_BytesToHexStr(ieee_addr, sizeof(ieee_addr), prompt_str);
        GUI_DispStr(data_buf, 6+strlen("0X")*GUI_FONT_WIDTH, 6, prompt_str, 1);
        GUI_DispPic(data_buf, 0, 8, bar_code, 200, 64);
        EPD_Init();
        EPD_Display(data_buf, 4736);
        EPD_Close();


        WaitMs(3000);


    }
}




