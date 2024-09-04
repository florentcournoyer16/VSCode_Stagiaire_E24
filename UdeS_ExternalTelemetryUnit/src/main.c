/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Peripheral Heart Rate over LE Coded PHY sample
 */
#include "cortical_implant_coord.h"		//Choose this .h if in COORD mode in the CMAKE
// #include "cortical_implant_node.h" 	//Choose this .h if in NODE mode in the CMAKE
#include <nrfx_log.h>
// #include "pairing_basic_node.h"		//Keep commented
// #include "pairing_basic_coord.h"		//Keep commented
#include "hw_cfg.h"
#include "INA231.h"
#include "usb_console.h"

/* Private function prototype ************************************************/

/* INA I2C DEVICES ************************************************************/
static const struct i2c_dt_spec dev_ina_MCU = I2C_DT_SPEC_GET(DT_NODELABEL(ina_mcu));
static const struct i2c_dt_spec dev_ina_UWB = I2C_DT_SPEC_GET(DT_NODELABEL(ina_uwb));
static const struct i2c_dt_spec dev_ina_uSD = I2C_DT_SPEC_GET(DT_NODELABEL(ina_usd));
static const struct i2c_dt_spec dev_ina_5V = I2C_DT_SPEC_GET(DT_NODELABEL(ina_5v));

void show_data_ina23x(struct ina23x_data *ina1);
void init_all_ina23x(struct ina23x_data *ina1,struct ina23x_data *ina2,
	struct ina23x_data *ina3,struct ina23x_data *ina4);


/* BLE related prototype ****************************************************************/
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HRS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_DIS_VAL))
};

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void);
static void auth_cancel(struct bt_conn *conn);

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

static void bas_notify(void);
static void hrs_notify(void);
/* END of BLE realted prototype *********************************************************/

int main(void)
{
	uint32_t err;
	uint32_t led_status = 0;
	struct ina23x_data ina_MCU = {dev_ina_MCU};
	struct ina23x_data ina_UWB = {dev_ina_UWB};
	struct ina23x_data ina_uSD = {dev_ina_uSD};
	struct ina23x_data ina_5V = {dev_ina_5V};
	if (!gpio_is_ready_dt(&led0) & !gpio_is_ready_dt(&led1) & !gpio_is_ready_dt(&led2) & !gpio_is_ready_dt(&led3) & !gpio_is_ready_dt(&led4) & !gpio_is_ready_dt(&uwb_irq_pin))
	{
		return 0;
	}

	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led4, GPIO_OUTPUT_INACTIVE);

	err = enable_usb_console();
	if (err)
	{
		printk("USB enabling failed (err %d)\n", err);
		return 0;
	}

	err = dk_leds_init();
	if (err)
	{
		printk("LEDs init allo failed (err %d)\n", err);
		return 0;
	}
	// Initializing all ina231 before use and power down them.
	init_all_ina23x(&ina_MCU, &ina_UWB, &ina_uSD, &ina_5V);

	iface_tx_conn_status();
    iface_delay(500);
    iface_rx_conn_status();
    iface_delay(500);
    iface_rx_conn_status();
    iface_delay(500);
    iface_tx_conn_status();

	init_cortical_implant();

	/* BLE code *******************************************/
	printk("Starting Bluetooth Peripheral HR coded example\n");
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	bt_ready();

	//bt_conn_auth_cb_register(&auth_cb_display);
	/*******************************************************/

	for (;;)
	{
		cortical_implant_routine();

		printk(GRN"***********************************************************************************\n");
		show_data_ina23x(&ina_MCU);
		show_data_ina23x(&ina_UWB);
		show_data_ina23x(&ina_uSD);
		show_data_ina23x(&ina_5V);
		printk(GRN"***********************************************************************************\n"NRM);

		/* Heartrate measurements simulation */
		hrs_notify();
		/* Battery level simulation */
		bas_notify();

		unpair_device();
		k_sleep(K_MSEC(100));
	}

	return 0;
}

/* Private function ***********************************************************/
void init_all_ina23x(struct ina23x_data *ina1,struct ina23x_data *ina2,
	struct ina23x_data *ina3,struct ina23x_data *ina4)
{
	int err = 0;
	// ina23x initialisation if available
	if (ina23x_available(ina1)){
		err += ina23x_init(ina1, 30000, 10);
		err += ina23x_alert_enable_set(ina1, INA231_CONVERSION_READY_BIT, 0, 0);
	}else{
		printk("ina23x MCU enabling failed (err %d)\n", err);
	}
	if (ina23x_available(ina2)){
		err += ina23x_init(ina2, 30000, 10);
		err += ina23x_alert_enable_set(ina2, INA231_CONVERSION_READY_BIT, 0, 0);
	}else{
		printk("ina23x UWB enabling failed (err %d)\n", err);
	}
	if (ina23x_available(ina3)){
		err += ina23x_init(ina3, 30000, 10);
		err += ina23x_alert_enable_set(ina3, INA231_CONVERSION_READY_BIT, 0, 0);
	}else{
		printk("ina23x uSD enabling failed (err %d)\n", err);
	}
	if (ina23x_available(ina4)){
		err += ina23x_init(ina4, 30000, 50);
		err += ina23x_alert_enable_set(ina4, INA231_CONVERSION_READY_BIT, 0, 0);
	}else{
		printk("ina23x 5V enabling failed (err %d)\n", err);
	}

	// Power down to save energy while not in use
	if(!ina23x_power_down(ina1)){
		printk("Error in ina231 power down (ina@%x!\n",ina1->devSpec.addr);
	}
	if(!ina23x_power_down(ina2)){
		printk("Error in ina231 power down (ina@%x!\n",ina2->devSpec.addr);
	}
		if(!ina23x_power_down(ina3)){
		printk("Error in ina231 power down (ina@%x!\n",ina3->devSpec.addr);
	}
		if(!ina23x_power_down(ina4)){
		printk("Error in ina231 power down (ina@%x!\n",ina4->devSpec.addr);
	}

	if (err){
		printk("Error in ina231 initialization !\n");
	}else{
		printk("All ina231 initialized !\n");
	}
}

void show_data_ina23x(struct ina23x_data *ina1){
	int err = 0;
	int tempCurrent = 0;
	int tempBus = 0;
	int tempPower = 0;

	// Power up before reading
	ina23x_power_up(ina1);

	// Read current, Bus voltage and power and print the values
	err += ina23x_format_read(ina1,INA23X_CURRENT,&tempCurrent);
	err += ina23x_format_read(ina1,INA23X_POWER,&tempPower);
	err += ina23x_format_read(ina1,INA23X_BUS_VOLTAGE,&tempBus);

	if(err){
		printk("Error showing the data (ina@%x)", ina1->devSpec.addr);
		// Power down to save energy
		ina23x_power_down(ina1);
	}else{
		switch(ina1->devSpec.addr){
		case 0x40:
			printk(GRN"ina@MCU : "NRM);
			break;
		case 0x41:
			printk(GRN"ina@UWB : "NRM);
			break;
		case 0x44:
			printk(GRN"ina@uSD : "NRM);
			break;
		case 0x45:
			printk(GRN"ina@5V  : "NRM);
			break;
		default:
			printk(YEL"ina@%x : "NRM, ina1->devSpec.addr);
			break;
		}
		printk("Bus voltage = %i mV || Current = %i uA \t|| Power = %i uW\n", tempBus, tempCurrent, tempPower);
	}

	// Power down after reading to save energy
	ina23x_power_down(ina1);
}

/* Bluetooth related functions *************************************************/

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static void bas_notify(void)
{
	uint8_t battery_level = bt_bas_get_battery_level();

	battery_level--;

	if (!battery_level) {
		battery_level = 100U;
	}

	bt_bas_set_battery_level(battery_level);
}

static void hrs_notify(void)
{
	static uint8_t heartrate = 90U;

	/* Heartrate measurements simulation */
	heartrate++;
	if (heartrate == 160U) {
		heartrate = 90U;
	}

	bt_hrs_notify(heartrate);
}