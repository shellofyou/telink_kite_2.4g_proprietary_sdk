## V3.3.1

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.3.1
* This version of SDK supports  B85 chips.
* The default configuration of LEDs and KEYs match the following hardware revisions:
*	B85	 	C1T139A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* change docs file and some info.
* Add and optimize uart upgrade firmware and OTA's crc verification.
* Add stx/srx/stx2rx/srx2tx routines of tpsll（Telink proprietary stack link layer）.

### Bug Fixes
* Solve the problem that  gotten  payload length is packet length but not real payload length in General fsk fix packet format.
* Sovle the problem of turning into bricks after power failure when firmware updating.
* Solve the problem of ble_ adv routine connection failure that causesd by driver updating rf step from 0.5 to 1.

### BREAKING CHANGES

* N/A

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.3.1
* 此版本SDK支持 B85 芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B85	 	C1T139A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* 修改docs文件夹和一些标注。
* 添加优化串口升级固件及ota的crc校验
* 添加tpsll（Telink proprietary stack link layer）的stx/srx/stx2rx/srx2tx例程。

### Bug Fixes
* 解决General fsk定长包获取的rx_payload_len为packet length而不是实际payload length的问题。
* 解决固件更新过程中断电变砖问题。
* 解决驱动更新rf step从0.5变为1导致ble_adv例程连接失败问题。

### BREAKING CHANGES

* N/A

## V3.3.0(PR)

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.3.0
* This version of SDK supports  B85 chips.
* The default configuration of LEDs and KEYs match the following hardware revisions:
*	B85	 	C1T139A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* Add two-point calibration to adc optimization.
* Add required user_read_flash_value_calib interface after cpu_wakeup_init function for VDDF\ADC and frequency offset calibration.
* Add uart_soft_rx\pwm_count\pwm_ir\pwm_ir_dma_fifo\pwm_ir_fifo routines.
* Add the functions of triggering interrupts on the rising or falling edge of gpio interrupts.
* Combine the original adc gpio sampling and adc Vbat sampling routines to the newly added adc_ sample routine.
* Add crc check of ota package.

### BREAKING CHANGES

* N\A

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.3.0
* 此版本SDK支持 B85 芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B85	 	C1T139A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* 新增adc校准优化的两点校准。
* 在初始化cpu_wakeup_init函数后增加必需的新增user_read_flash_value_calib接口用于 VDDF、ADC、频偏校准。
* 新增uart_soft_rx\pwm_count\pwm_ir\pwm_ir_dma_fifo\pwm_ir_fifo例程。
* 新增gpio上升沿或下降沿触发中断功能。
* 将原有的adc的gpio采样、adc的Vbat采样例程合并至新增adc_sample例程中。
* 增加ota包的crc校验。

### BREAKING CHANGES

* N\A

## V3.2.0

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.2.0
### Features
* Change the max packet length of payload in tpll mode from 32 bytes to 63 bytes.
* Put rf_rx_buffer_set、rf_trx_state_set、rf_start_stx、rf_set_channel_500k functions into .ram_code section.
* Add .sdk_version section in boot.link to store version info.

### BREAKING CHANGES
* N/A

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.2.0

### Features
* 将tpll模式下payload的最大包长度从32byte改为63byte。
* 将rf_rx_buffer_set、rf_trx_state_set、rf_start_stx、rf_set_channel_500k函数放入ramcode段。
* 在boot.link新增.sdk_version段存放版本信息。

### BREAKING CHANGES
* N/A

## V3.1.8

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.8.
### Features
* Add rf_pa diver.
* Add gen_fsk_pa routine.
* Fix tpll packet valid judge.

### BREAKING CHANGES
* N/A

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.8.

### Features
* 增加rf_pa驱动。
* 增加gen_fsk_pa例程。
* 修正tpll包有效判断。

### BREAKING CHANGES
* N/A


## V3.1.7

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.7.
* This version sdk support zbit flash..
### Features
* Set gen_ fsk_ tx\rx_ settle_ set function in the ramcode segment.
* In TPLL_ DateTypeDef adds 250k and 500k Daterate options.


### Known issues
* **UART** 
  - Add uart_ Dma routines.
* **TPLL** 
  - Updated the ReadRxPayload return value of Length.
### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.7.
* 此版本SDK支持KiteA3芯片.
### Features
* 将gen_fsk_tx\rx_settle_set函数放入ramcode段。
* 在TPLL_DaterateTypeDef增加了250k、500kDaterate选择。


### Known issues
* **UART** 
  - 增加uart_dma例程。
* **TPLL** 
  - 更新了TPLL_ReadRxPayload的返回值长度。
### BREAKING CHANGES
* N/A




## V3.1.6

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.6.
* This version sdk support zbit flash..
### Features
* Support zbit flash.
* For 24m clock calibration after waking up in deep_retention mode，called the API **rc_24m_cal** in the demo deep_32K_time_wakeup.
* Added the function of frequency offset calibration value writing to flash for customer reference in the demo ble_beacon demo. 
* Update and improve OTA demo function.
* The voltage detection content is added before the flash operation, and the minimum voltage of power is 2.0V.


### Known issues
* **USB** 
  - Add the demo usb_demo.
* **FLASH** 
  - Delet the demo flsh_write_protect, and update the demo flsh_operation.
* **lib** 
  - The library version number is tpll_0.0.1 and genfsk_0.0.1.
### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.6.
* 此版本SDK支持KiteA3芯片.
### Features
* 支持Zbit flash。
* 在deep_32K_time_wakeup demo中增加了接口rc_24m_cal ();，用于deep_retention模式醒来后进行24M时钟校准。
* 在ble_beacon demo中增加了频偏校准写入flash的操作以供客户参考。
* 更新并完善OTA demo功能。
* 在使用flash的读写功能前加了电压检测内容，最低供电电压为2.0V。 
### Known issues
* **USB** 
  - 增加usb_demo,并包括usb_cdc demo
* **FLASH** 
  - 更新flsh_operation demo ,删除了flsh_write_protect demo
* **库版本号** 
  - tpll_0.0.1与genfsk_0.0.1的库版本号不做改动。

### BREAKING CHANGES
* N/A





## V3.1.5

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.5.
* This version sdk support kite A3 chip.
### Features
* RF/TPLL
* Interface:RF/TPLL
### Known issues
* **rf** 
  - Add the ble no pn demo
* **tpll** 
  - Add the api to set and read the preamble length
* **lib** 
  - Increase the record of the library version number of tpll_0.0.1 and genfsk_0.0.1

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.5.
* 此版本SDK支持KiteA3芯片.
### Features
* RF/TPLL
* Interface:RF/TPLL
### Known issues
* **rf** 
  - 增加ble no pn demo
* **tpll** 
  - 增加设置跟获取preamble长度接口
* **库版本号** 
  - 增加记录tpll_0.0.1与genfsk_0.0.1的库版本号的记录

### BREAKING CHANGES
* N/A


## V3.1.4

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.4.
* This version sdk support kite chip.
### Features
* UARTOTA/RFOTA
* Interface:UART/RF
### Known issues
* **ota** 
  - Before using the OTA function, you need to open the corresponding macros in ota.c and fw_update.c

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.4.
* 此版本SDK支持Kite芯片.
### Features
* UARTOTA/RFOTA
* Interface:UART/RF
### Known issues
* **ota** 
   - 使用OTA功能之前，需要将对应的ota.c与fw_update.c里面的宏打开

### BREAKING CHANGES
* N/A


## V3.1.3

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.3.
* This version sdk support kite chip.
### Features
* USB
* Interface:USBCDC
### Known issues
* **usbcdc** 
  - Before using the USB CDC function, you need to install the driver files under Demo first

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.3.
* 此版本SDK支持Kite芯片.
### Features
* USB
* Interface:USBCDC
### Known issues
* **usbcdc** 
   - 在使用USB CDC功能之前，需要先安装Demo下面的驱动文件

### BREAKING CHANGES
* N/A



## V3.1.2

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.2.
* This version sdk support kite chip.
### Features
* PM
* Interface:PMLONGWAKEUP
### Known issues
* **pm** 
  - Added PM long sleep function interface

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.2.
* 此版本SDK支持Kite芯片.
### Features
* PM
* Interface:PMLONGWAKEUP
### Known issues
* **pm** 
   - 增加了PM长睡眠函数接口

### BREAKING CHANGES
* N/A


## V3.1.1

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.1.
* This version sdk support kite chip.
### Features
* TPLL
* Interface:TPLL
### Known issues
* **tpll** 
  - Cleaned up the code of TPLL module

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.1.
* 此版本SDK支持Kite芯片.
### Features
* TPLL
* Interface:TPLL
### Known issues
* **tpll** 
   - 清理了TPLL模块的代码

### BREAKING CHANGES
* N/A


## V3.1.0

### Version
* SDK version : telink_kite_2.4g_proprietary_sdk V3.1.0.
* This version sdk support kite chip.
### Features
* FLASH
* Interface:FLASH
### Known issues
* **flash** 
  - add flash customize address function

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_kite_2.4g_proprietary_sdk V3.1.0.
* 此版本SDK支持Kite芯片.
### Features
* FLASH
* Interface:FLASH
### Known issues
* **flash** 
   - 添加Flash自定义地址功能

### BREAKING CHANGES
* N/A










