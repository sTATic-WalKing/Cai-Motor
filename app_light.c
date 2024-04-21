#include <drivers/8258/gpio_8258.h>
#include "app_config.h"
#include "common/types.h"
#include "tl_common.h"
#include "drivers.h"

static unsigned int value[6] = { 0 };
extern u8 ctrl[2];
static unsigned int cmp[3] = { 0, 333, 666 };

void app_motor_restart(unsigned int current) {
    static unsigned int before;
    if (before != current) {
        app_uart_tx("restart\n", 9);
        pwm_stop(PWM4_ID);
        before = current;
        pwm_set_cycle_and_duty(PWM4_ID, (u16)(1000 * CLOCK_SYS_CLOCK_1US), (u16)(before * CLOCK_SYS_CLOCK_1US));
        pwm_start(PWM4_ID);
    }
}

void app_light_init() 
{
    value[4] = 1;
    value[5] = 0;

    pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

    gpio_set_func(GPIO_PC2, AS_GPIO);
	gpio_set_func(GPIO_PC3, AS_GPIO);
	gpio_set_func(GPIO_PC4, AS_GPIO);
	gpio_set_func(GPIO_PB4, AS_PWM4);
	gpio_set_func(GPIO_PB5, AS_GPIO);
	gpio_set_func(GPIO_PB6, AS_GPIO);

	gpio_set_output_en(GPIO_PC2, 1);
	gpio_set_output_en(GPIO_PC3, 1);
	gpio_set_output_en(GPIO_PC4, 1);
	gpio_set_output_en(GPIO_PB4, 1);
	gpio_set_output_en(GPIO_PB5, 1);
	gpio_set_output_en(GPIO_PB6, 1);

	gpio_set_input_en(GPIO_PC2, 0); 
	gpio_set_input_en(GPIO_PC3, 0); 
	gpio_set_input_en(GPIO_PC4, 0); 
	gpio_set_input_en(GPIO_PB4, 0); 
	gpio_set_input_en(GPIO_PB5, 0); 
	gpio_set_input_en(GPIO_PB6, 0); 

	gpio_set_func(GPIO_PD2, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_PD2, PM_PIN_PULLUP_10K);
	gpio_set_output_en(GPIO_PD2, 0);
	gpio_set_input_en(GPIO_PD2, 1);

    pwm_set_mode(PWM4_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM4_ID, 0);
	pwm_set_cycle_and_duty(PWM4_ID, (u16)(1000 * CLOCK_SYS_CLOCK_1US), (u16)(0 * CLOCK_SYS_CLOCK_1US));
	pwm_start(PWM4_ID);
}

void app_motor_state(unsigned int state)
{
    ctrl[1] = state;
    value[MOTOR_CMP] = cmp[ctrl[1]];
    bls_att_pushNotifyData(CAT_CTRL_VALUE_H, ctrl, sizeof(ctrl));
}

void app_motor_click()
{
    ++ctrl[1];
    if (ctrl[1] == sizeof(cmp) / sizeof(unsigned int)) {
        ctrl[1] = 0;
    }
    value[MOTOR_CMP] = cmp[ctrl[1]];
    bls_att_pushNotifyData(CAT_CTRL_VALUE_H, ctrl, sizeof(ctrl));
}

void app_light_adv()
{
    value[LIGHT_BLUE] = 1;
	value[LIGHT_GREEN] = 0;
    value[LIGHT_RED] = 0;
}

void app_light_error()
{
    value[LIGHT_BLUE] = 0;
	value[LIGHT_GREEN] = 0;
    value[LIGHT_RED] = 1;
}

void app_light_connect()
{
    value[LIGHT_BLUE] = 0;
	value[LIGHT_GREEN] = 1;
    value[LIGHT_RED] = 0;
}

static unsigned int before = 1;
void app_motor_loop() 
{
    gpio_write(GPIO_PC2, value[0]); 
	gpio_write(GPIO_PC3, value[1]); 
	gpio_write(GPIO_PC4, value[2]); 
    app_motor_restart(value[MOTOR_CMP]);
	gpio_write(GPIO_PB5, value[4]);
    gpio_write(GPIO_PB6, value[5]);

    unsigned int current = gpio_read(GPIO_PD2);
    if (before == 0 && current != 0) {
        app_uart_tx("clicked\n", 9);
        app_motor_click();
    }
    before = current;
}