#include "driver.h"
#include "common.h"
#include "ble_adv.h"


#define    DEEP_DURATION        1000 //unit: ms

__attribute__((section(".retention_data"))) volatile unsigned char deep_wkup_flg = 0;

_attribute_ram_code_sec_noinline_ int main(void) 
{
    unsigned char tx_payload[64];
    int i = 0; 

    //system and clock init
//	blc_pm_select_external_32k_crystal();
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_shutdown(GPIO_ALL); //set all unused GPIOs during deep sleep as high_Z state, avoiding current leakage

    tx_payload[0] = 0x02; //header0
    tx_payload[1] = sizeof(tx_payload) - 2; //header1
    for (i = 0; i < (sizeof(tx_payload) - 2); i++) {
        tx_payload[i+2] = i;
    }

    //config radio and send data packet
    rfid_radio_config();
    rfid_packet_send(tx_payload, sizeof(tx_payload));

#if CLK_32K_XTAL_EN
    //goto deep sleep
    if (deep_wkup_flg != 1) {
        deep_wkup_flg = 1;
        clock_32k_init(CLK_32K_XTAL);
    }
#endif
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_TIMER, ClockTime()+DEEP_DURATION*1000*16);

    while (1) {
        
    }
}
