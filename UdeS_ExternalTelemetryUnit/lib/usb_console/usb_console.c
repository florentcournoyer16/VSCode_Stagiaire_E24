/** @file       usb_console.h
 *  @brief      USB console librairy.
 * 
 *  @copyright  Copyright (c) 2016 Intel Corporation.
 *
 *  @author     Adapted by Jérémy Ménard from nRF sample code.
 *  @date       Created on december 12th, 2023, 13:59 PM
 */

/* INCLUDES *******************************************************************/
#include "usb_console.h"

/* PUBLIC FUNCTIONS ***********************************************************/

/** @brief Initialize USB console.
 */

int enable_usb_console(void)
{
    const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

    #if defined(CONFIG_USB_DEVICE_STACK_NEXT)
        if (enable_usb_device_next()) {
            return 1;
        }
    #else
        if (usb_enable(NULL)) {
            return 2;
        }
    #endif

	/* Poll if the DTR flag was set */
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */
		k_sleep(K_MSEC(100));
	}
    return 0;
}
