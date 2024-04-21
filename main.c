#include <stack/ble/ble.h>

_attribute_my_ram_code_ void irq_handler(void)
{
	app_uart_irq();
	irq_blt_sdk_handler();
}

_attribute_ram_code_ int main (void)
{
    random_generator_init();
	blc_pm_select_internal_32k_crystal();
	cpu_wakeup_init();
    rf_drv_init(RF_MODE_BLE_1M);
	gpio_init(1);
	clock_init(SYS_CLK_24M_Crystal);
	blc_app_loadCustomizedParameters();

	app_uart_init();
    app_light_init();
    app_ble_init();
	irq_enable();

    app_uart_tx("while\n", 7);
	while (1) 
    {
        app_motor_loop();
		app_ble_loop();
	}
}

