#include "driver.h"
#include "common.h"


#define    GREEN_LED_PIN        GPIO_PD3
#define    ADC_INPUT_PIN        GPIO_PB0
#define    DEEP_DURATION        998 //unit: ms

_attribute_session_(".retention_data") volatile static unsigned int rfid_packet_cnt = 0;

_attribute_ram_code_sec_noinline_ int main (void) 
{
    unsigned char rfid_payload[8];
    unsigned short battery_volt;

    //system and clock init
//	blc_pm_select_external_32k_crystal();
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //blink the led
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1);
    gpio_write(GREEN_LED_PIN, 1);

    //do battery volt sample
    adc_init();
    adc_vbat_init(ADC_INPUT_PIN);
    adc_power_on_sar_adc(1);
    battery_volt = adc_sample_and_get_result();

    //prepare the packet payload
    rfid_payload[0] = battery_volt & 0xff;
    rfid_payload[1] = (battery_volt>>8) & 0xff;
    rfid_payload[2] = rfid_packet_cnt & 0xff;
    rfid_payload[3] = (rfid_packet_cnt>>8) & 0xff;
    rfid_payload[4] = (rfid_packet_cnt>>16) & 0xff;
    rfid_payload[5] = (rfid_packet_cnt>>24) & 0xff;
    rfid_payload[6] = 0xaa;
    rfid_payload[7] = 0xbb;

    rfid_packet_cnt++;

    //config radio and send data packet
    rfid_radio_config();
    rfid_packet_send(rfid_payload, sizeof(rfid_payload));

    //goto deep sleep
    gpio_shutdown(GPIO_ALL); //set all unused GPIOs during deep sleep as high_Z state, avoiding current leakage
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_TIMER, ClockTime()+DEEP_DURATION*1000*16);

    while (1) {
        
    }
}
