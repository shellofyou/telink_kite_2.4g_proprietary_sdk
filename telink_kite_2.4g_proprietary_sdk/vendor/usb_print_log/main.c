#include "driver.h"


unsigned char debug_logo[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

int main(void)
{
    cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    usb_set_pin_en();
    usb_loginit();
    WaitMs(3000); //delay to ensure USB enumerate done


    while (1) {

    	log_msg("logo", debug_logo, sizeof(debug_logo));

        WaitMs(1000);
    }
}
