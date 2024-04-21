#include <tl_common.h>
#include <stack/ble/ble.h>
#include "app_config.h"

static const u16 primary_service = GATT_UUID_PRIMARY_SERVICE;
static const u16 character = GATT_UUID_CHARACTER;

static const u16 gap_service = SERVICE_UUID_GENERIC_ACCESS;
static const u8 gap_device_name_character[] = {
	CHAR_PROP_READ,
	U16_LO(GAP_DEVICE_NAME_VALUE_H), U16_HI(GAP_DEVICE_NAME_VALUE_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const u16 gap_device_name = GATT_UUID_DEVICE_NAME;
static const u8 device_name[] = { BLE_NAME };

static const u16 gatt_service = SERVICE_UUID_GENERIC_ATTRIBUTE;
static const u8 gatt_service_change_character[] = {
	CHAR_PROP_INDICATE,
	U16_LO(GATT_SERVICE_CHANGED_VALUE_H), U16_HI(GATT_SERVICE_CHANGED_VALUE_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};
static const u16 gatt_service_change = GATT_UUID_SERVICE_CHANGE;
static u16 service_change[2] = { 0 };

static const u16 common_CCCD = GATT_UUID_CLIENT_CHAR_CFG;
static u8 CCCD[2] = { 0, 0 };

static const u8 cai_service[16] = { CAI_SERVICE };
static const u8 cai_ctrl_character[] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_INDICATE,
	U16_LO(CAT_CTRL_VALUE_H), U16_HI(CAT_CTRL_VALUE_H),
	CAI_CTRL,
};
static const u8 cai_ctrl[16] = { CAI_CTRL };
u8 ctrl[2] = { 0x01, 0x00 };
static int cai_ctrl_write_callback(void *p) 
{
    rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
    int len = pw->l2cap - 3;

    if (len <= 0) { return 1; }

    if (pw->dat[0] > 2) {
        pw->dat[0] = 2;
    }
    app_motor_state(pw->dat[0]);

    return 1;
}

static const attribute_t attributes[] = {
	{ ATT_H_END - 1, 0 ,0 ,0 ,0 ,0 },

	{ 3, ATT_PERMISSIONS_READ, 2, 2, (u8*)(&primary_service), (u8*)(&gap_service), 0 },
	{ 0, ATT_PERMISSIONS_READ, 2, sizeof(gap_device_name_character), (u8*)(&character), (u8*)(gap_device_name_character), 0},
	{ 0, ATT_PERMISSIONS_READ, 2, sizeof(device_name), (u8*)(&gap_device_name), (u8*)(device_name), 0 },

	{ 4, ATT_PERMISSIONS_READ, 2, 2, (u8*)(&primary_service), (u8*)(&gatt_service), 0 },
	{ 0, ATT_PERMISSIONS_READ, 2, sizeof(gatt_service_change_character), (u8*)(&character), (u8*)(gatt_service_change_character), 0 },
	{ 0, ATT_PERMISSIONS_READ, 2, sizeof(service_change), (u8*)(&gatt_service_change), (u8*)(&service_change), 0 },
    { 0, ATT_PERMISSIONS_RDWR, 2, sizeof(CCCD), (u8*)(&common_CCCD), (u8*)(CCCD), 0},

    { 4, ATT_PERMISSIONS_READ, 2, 16, (u8*)(&primary_service), (u8*)(&cai_service), 0},
    { 0, ATT_PERMISSIONS_READ, 2, sizeof(cai_ctrl_character), (u8*)(&character), (u8*)(cai_ctrl_character), 0},
	{ 0, ATT_PERMISSIONS_RDWR, 16, sizeof(ctrl), (u8*)(&cai_ctrl), ctrl, &cai_ctrl_write_callback, 0 },
    { 0, ATT_PERMISSIONS_RDWR, 2, sizeof(CCCD), (u8*)(&common_CCCD), (u8*)(CCCD), 0},
};

void app_att_init()
{
	bls_att_setAttributeTable((u8*)attributes);
}


