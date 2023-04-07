/********************************************************************************************************
 * @file	printf.h
 *
 * @brief	This is the header file for B85
 *
 * @author	Driver Group
 * @date	2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "gpio.h"
#pragma once

#define DEBUG_MODE 1

#if(DEBUG_MODE==1)

#define  DEBUG_IO		1
#define  DEBUG_USB		2

#define  DEBUG_BUS  	1

#if(DEBUG_BUS==DEBUG_IO)
#define PRINT_BAUD_RATE             		115200   	//1M baud rate,should Not bigger than 1Mb/s
#define DEBUG_INFO_TX_PIN           		GPIO_PB1
#define TX_PIN_GPIO_EN()					gpio_set_func(DEBUG_INFO_TX_PIN , AS_GPIO);
#define TX_PIN_PULLUP_1M()					gpio_setup_up_down_resistor(DEBUG_INFO_TX_PIN, PM_PIN_PULLUP_1M);
#define TX_PIN_OUTPUT_EN()					gpio_set_output_en(DEBUG_INFO_TX_PIN,1)
#define TX_PIN_OUTPUT_REG					(0x583+((DEBUG_INFO_TX_PIN>>8)<<3))
#endif
/**
 * @brief  This function initializes the USB interface for printing log message
 * @param  none
 * @return none
 */
extern void usb_loginit(void);

/**
 * @brief     This function serves to print the log message via USB interface
 * @param[in] Str - pointer to the prompting string of the log message
 * @param[in] Content - pointer to the actual content of the log message
 * @return none
 */
extern void log_msg(char *Str, unsigned char *Content, int Len);


extern void tl_printf(const char *format, ...);
#define printf		tl_printf

#else

static inline void no_printf(const char *format, ...){    return;}
#define printf		no_printf

#endif

extern void tl_sprintf(char *buff, const char *format, ...);
#define sprintf		tl_sprintf

