#ifndef HW_CFG_H_
#define HW_CFG_H_

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#define DEVICE_NAME             CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED          0
#define CON_STATUS_LED          1
#define REC_STATUS_LED          2
#define RUN_LED_BLINK_INTERVAL  1000
#define NOTIFY_INTERVAL         1000

/* *********************************************** */
/************** GPIO CONGIGURATION *****************/

/* Outputs */

/* The devicetree node identifier for the GPIOS alias. */
#define LED0_NODE           DT_ALIAS(led0)
#define LED1_NODE           DT_ALIAS(led1)
#define LED2_NODE           DT_ALIAS(led2)
#define LED3_NODE           DT_ALIAS(led3)
#define LED4_NODE           DT_ALIAS(led4)
#define UWB_SHTDWN_NODE     DT_NODELABEL(uwb_shutdown)
#define UWB_RESET_NODE      DT_NODELABEL(uwb_reset)
#define UWB_CS_NODE			DT_NODELABEL(uwb_cs)
/* GPIOs spec structure containing the configuration. */
/* Arguments of functions named : gpio_pin_XXX_dt() */
static const struct gpio_dt_spec led0           = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1           = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2           = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3           = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec led4           = GPIO_DT_SPEC_GET(LED4_NODE, gpios);
static const struct gpio_dt_spec uwb_shutdown_pin   = GPIO_DT_SPEC_GET(UWB_SHTDWN_NODE, gpios);
static const struct gpio_dt_spec uwb_reset_pin      = GPIO_DT_SPEC_GET(UWB_RESET_NODE, gpios);

/* Inputs */

/* The devicetree node identifier for the GPIOS alias. */
#define UWB_IRQ_NODE DT_ALIAS(uwb_irq)

/* GPIOs spec structure containing the configuration. */
/* To use with function named : gpio_pin_XXX_dt() */
static const struct gpio_dt_spec uwb_irq_pin    = GPIO_DT_SPEC_GET(UWB_IRQ_NODE, gpios);

/* *********************************************** */

#define SPI_UWB_NODE 		DT_NODELABEL(spi3)
#define SPI_UWB_OPERATION 	SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB

static const struct device *const uwb_spi_dev = DEVICE_DT_GET(SPI_UWB_NODE);


// static struct spi_dt_spec uwb_spi_dev = SPI_CONFIG_DT(SPI_UWB_NODE, SPI_UWB_OPERATION, 0);


static struct spi_cs_control uwb_cs_ctrl = (struct spi_cs_control){
		.gpio = GPIO_DT_SPEC_GET(UWB_CS_NODE, gpios),
		.delay = 0u,
};

// static struct spi_cs_control uwb_cs_ctrl = (struct spi_cs_control){
// 		.gpio = GPIO_DT_SPEC_GET(SPI_UWB_NODE, cs_gpios),
// 		.delay = 0u,
// };

#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"



/* IRQ Lines */
//#define PEND_SV_IRQ 		20

/* Priorities */
//#define PRIO_PEND_SV_IRQ	2


#endif // HW_CFG_H_