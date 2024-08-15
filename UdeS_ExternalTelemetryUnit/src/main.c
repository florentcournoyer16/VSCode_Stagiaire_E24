/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Peripheral Heart Rate over LE Coded PHY sample
 */
// #include "cortical_implant_coord.h"
#include "cortical_implant_node.h"
#include <dk_buttons_and_leds.h>
#include <errno.h>
#include <nrfx_log.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>
// #include "pairing_basic_node.h"
// #include "pairing_basic_coord.h"
#include "hw_cfg.h"
#include "usb_console.h"
/* Private function prototype ************************************************/

int main(void)
{
	uint32_t err;
	uint32_t led_status = 0;
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
		printk("LEDs init failed (err %d)\n", err);
		return 0;
	}

	iface_tx_conn_status();
    iface_delay(500);
    iface_rx_conn_status();
    iface_delay(500);
    iface_rx_conn_status();
    iface_delay(500);
    iface_tx_conn_status();

	cortical_implant_routine();

	for (;;)
	{
		dk_set_led(RUN_STATUS_LED, (++led_status) % 2);
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
	}

	return 0;
}
