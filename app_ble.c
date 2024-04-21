#include <stack/ble/ble.h>
#include "app_config.h"

#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8
#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16

_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};

_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};

static void adv() 
{
    u8 status = bls_ll_setAdvEnable(1);
    if (status == BLE_SUCCESS) 
    {
        app_light_adv();
    }
    else
    {
        app_light_error();
        while(1);
    }
}

static void connect() 
{
    u8 status = bls_ll_setAdvEnable(0);
    if (status == BLE_SUCCESS) 
    {
        app_light_connect();
    }
    else
    {
        app_light_error();
        while(1);
    }
}

static void connect_callback (u8 e, u8 *p, int n)
{
    connect();
	bls_l2cap_requestConnParamUpdate(8, 8, 99, 400);
}

static void terminate_callback (u8 e, u8 *p, int n)
{
    adv();
}

static u8 mac_public[6];
static u8 mac_static[6];
static u8 adv_data[] = { 
    BLE_NAME_SIZE + 1, 0x09, BLE_NAME,
    0x11, 0x07, CAI_SERVICE };

void app_ble_init()
{
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_static);

    blc_ll_initBasicMCU();
	blc_ll_initStandby_module(mac_public);
	blc_ll_initAdvertising_module(mac_public);
	blc_ll_initConnection_module();
	blc_ll_initSlaveRole_module();
    blc_gap_peripheral_init();
    
	app_att_init();

	blc_l2cap_register_handler(blc_l2cap_packet_receive);
    bls_ll_setAdvData(adv_data, sizeof(adv_data));
    bls_ll_setScanRspData(NULL, 0);
    bls_ll_setAdvParam(
        ADV_INTERVAL_50MS, 
        ADV_INTERVAL_50MS, 
        ADV_TYPE_CONNECTABLE_UNDIRECTED, 
        OWN_ADDRESS_PUBLIC, 
        0, NULL, 
        BLT_ENABLE_ADV_ALL, 
        ADV_FP_NONE);
    rf_set_power_level_index(RF_POWER_P0p04dBm);
    bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &connect_callback);
	bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE, &terminate_callback);
    adv();
}

void app_ble_loop()
{
    blt_sdk_main_loop();
}