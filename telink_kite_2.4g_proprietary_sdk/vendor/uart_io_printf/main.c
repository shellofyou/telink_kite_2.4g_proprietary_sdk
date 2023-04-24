#include "driver.h"


#define trans_buff_Len    16


__attribute__((aligned(4))) unsigned char trans_buff[trans_buff_Len] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, \
                                                                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00};


_attribute_ram_code_sec_noinline_ int main (void)
{
    cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);


    while (1) {
    //use DEBUG_IO mdoe in printf.h
    printf(trans_buff);
	WaitMs(1000);


    }
}
