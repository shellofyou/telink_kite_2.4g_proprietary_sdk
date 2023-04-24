#include "driver.h"

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ      24000000   //  24m

/* List tick per second/millisecond/microsecond */


//normal mode
#define  TEST_PWM_NORMAL_MODE_1                1
#define  TEST_PWM_NORMAL_MODE_2                2
#define  TEST_PWM_NORMAL_MODE_3                3

#define  TEST_PWM_SELECT                    TEST_PWM_NORMAL_MODE_3

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,				///< system tick per 1 second
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),		///< system tick per 1 millisecond
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),   ///< system tick per 1 microsecond
};
/*********************************************************************************
    PWM0   :  PA2.  PC1.  PC2.    PD5
    PWM1   :  PA3.  PC3.
    PWM2   :  PA4.  PC4.
    PWM3   :  PB0.  PD2.
    PWM4   :  PB1.  PB4.
    PWM5   :  PB2.  PB5.
    PWM0_N :  PA0.  PB3.  PC4    PD5
    PWM1_N :  PC1.  PD3.
    PWM2_N :  PD4.
    PWM3_N :  PC5.
    PWM4_N :  PC0.  PC6.
    PWM5_N :  PC7.
 *********************************************************************************/

void app_pwm_test(void)
{
    pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

#if (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_1)  //test PWMx (0~5)   normal mode
    //PA2 PWM0  1ms cycle  1/2 duty
    gpio_set_func(GPIO_PA2, AS_PWM0);
    pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM0_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (500 * CLOCK_SYS_CLOCK_1US));
    pwm_start(PWM0_ID);

    //PA3 PWM1  1ms cycle  1/3 duty
    gpio_set_func(GPIO_PA3, AS_PWM1);
    pwm_set_mode(PWM1_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM1_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM1_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (333 * CLOCK_SYS_CLOCK_1US) );
    pwm_start(PWM1_ID);

    //PA4 PWM2   1ms cycle  1/4 duty
    gpio_set_func(GPIO_PA4, AS_PWM2);
    pwm_set_mode(PWM2_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM2_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM2_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (250 * CLOCK_SYS_CLOCK_1US) );
    pwm_start(PWM2_ID);

    //PB0 PWM3  1ms cycle  1/5 duty
    gpio_set_func(GPIO_PB0, AS_PWM3);
    pwm_set_mode(PWM3_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM3_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM3_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (200 * CLOCK_SYS_CLOCK_1US) );
    pwm_start(PWM3_ID);

    //PB1 PWM4  1ms cycle  2/3 duty
    gpio_set_func(GPIO_PB1, AS_PWM4);
    pwm_set_mode(PWM4_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM4_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM4_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (667 * CLOCK_SYS_CLOCK_1US) );
    pwm_start(PWM4_ID);

    //PB2 PWM5  1ms cycle  3/4 duty
    gpio_set_func(GPIO_PB2, AS_PWM5);
    pwm_set_mode(PWM5_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM5_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM5_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (750 * CLOCK_SYS_CLOCK_1US) );
    pwm_start(PWM5_ID);

#elif (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_2)  //test PWMx and PWMx_N(0~2)   normal mode

    //PC1 PWM0     1ms cycle  1/3 duty
    //PA0 PWM0_N   1ms cycle  2/3 duty
    gpio_set_func(GPIO_PA2, AS_PWM0);
    gpio_set_func(GPIO_PD5, AS_PWM0_N);
    pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM0_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (333 * CLOCK_SYS_CLOCK_1US) );

    //PC3 PWM1     1ms cycle  1/4 duty
    //PD3 PWM1_N   1ms cycle  3/4 duty
    gpio_set_func(GPIO_PA3, AS_PWM1);
    gpio_set_func(GPIO_PD3, AS_PWM1_N);
    pwm_set_mode(PWM1_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM1_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM1_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (250 * CLOCK_SYS_CLOCK_1US) );

    //PC4 PWM2     1ms cycle  1/5 duty
    //PD4 PWM2_N   1ms cycle  4/5 duty
    gpio_set_func(GPIO_PA4, AS_PWM2);
    gpio_set_func(GPIO_PD4, AS_PWM2_N);
    pwm_set_mode(PWM2_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM2_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM2_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (200 * CLOCK_SYS_CLOCK_1US) );

    pwm_start(PWM0_ID);
    pwm_start(PWM1_ID);
    pwm_start(PWM2_ID);

#elif (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_3)  //test PWMx and PWMx_N(3~5)   normal mode

    //PD2 PWM3     1ms cycle  1/3 duty
    //PC5 PWM3_N   1ms cycle  2/3 duty
    gpio_set_func(GPIO_PD2, AS_PWM3);
    gpio_set_func(GPIO_PC5, AS_PWM3_N);
    pwm_set_mode(PWM3_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM3_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM3_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (333 * CLOCK_SYS_CLOCK_1US) );

    //PB4 PWM4     1ms cycle  1/4 duty
    //PC0 PWM4_N   1ms cycle  3/4 duty
    gpio_set_func(GPIO_PB4, AS_PWM4);
    gpio_set_func(GPIO_PC6, AS_PWM4_N);
    pwm_set_mode(PWM4_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM4_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM4_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (250 * CLOCK_SYS_CLOCK_1US) );

    //PB5 PWM5     1ms cycle  1/5 duty
    //PC7 PWM5_N   1ms cycle  4/5 duty
    gpio_set_func(GPIO_PB5, AS_PWM5);
    gpio_set_func(GPIO_PC7, AS_PWM5_N);
    pwm_set_mode(PWM5_ID, PWM_NORMAL_MODE);
    pwm_set_phase(PWM5_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM5_ID, (unsigned short) (1000 * CLOCK_SYS_CLOCK_1US),  (unsigned short) (200 * CLOCK_SYS_CLOCK_1US) );

    pwm_start(PWM3_ID);
    pwm_start(PWM4_ID);
    pwm_start(PWM5_ID);

#else


#endif

}

int main (void)
{

	cpu_wakeup_init();

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    app_pwm_test();

    while (1) {

    }
}



