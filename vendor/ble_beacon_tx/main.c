#include "driver.h"
#include "common.h"
#include "ble_adv.h"


#define RF_POWER RF_POWER_P0p04dBm
#define CAP_VALUE  0x77000
unsigned char cap;
static unsigned char test_pdu[] = {0x02, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                                   0x05, 0x09, 0x74, 0x48, 0x49, 0x44, 0x02, 0x01,
                                   0x05};
int main(void)
{
	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);


    ble_adv_init(37, RF_POWER);

    flash_read_page(CAP_VALUE,1,&cap);
    if(cap!=0xff)
	{
    	rf_update_internal_cap(cap);
	}
    while (1) {
        ble_adv_send(test_pdu, sizeof(test_pdu));
        WaitMs(5);
    }
}
