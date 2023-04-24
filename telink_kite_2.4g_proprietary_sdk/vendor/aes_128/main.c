#include "driver.h"

#define    GREEN_LED_PIN                      GPIO_PD3

unsigned char Encrypt_data[13];
unsigned char Decrypt_data[13];

unsigned char aes_init[13] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                              0x09, 0x0a, 0x0b, 0x0c, 0};

unsigned char AES_Key[16] = {0xc0, 0xc1, 0xc2, 0xc3,
							 0xc4, 0xc5, 0xc6, 0xc7,
							 0xc8, 0xc9, 0xca, 0xcb,
							 0xcc, 0xcd, 0xce, 0xcf};


static void  user_init(void)
{

    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_set_input_en(GREEN_LED_PIN, 0); //disable input
    gpio_write(GREEN_LED_PIN, 0); //LED Off


}
int main(void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    user_init();


    while (1) {

        aes_init[0]++;

        aes_encrypt(AES_Key,aes_init,Encrypt_data);
        aes_decrypt(AES_Key,Encrypt_data,Decrypt_data);

        gpio_toggle(GREEN_LED_PIN);
        WaitMs(500); //delay 500 ms
    }
}
