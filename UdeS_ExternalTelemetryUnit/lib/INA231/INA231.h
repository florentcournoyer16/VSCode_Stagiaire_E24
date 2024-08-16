/** @file       INA231.h
 *  @brief      INA231 control function.
 * 
 *  @copyright  Copyright (c) 2016 Intel Corporation.
 *
 *  @author     Jérémy Ménard.
 *  @date       Created on december 13th, 2023, 10:40 PM
 */

#ifndef INA231_H_
#define INA231_H_

/* INCLUDES *******************************************************************/
#include <zephyr/drivers/i2c.h>
#include <float.h>
#include <string.h>
#include <math.h>


/* common register definitions */
#define INA23X_CONFIG			0x00
#define INA23X_SHUNT_VOLTAGE	0x01 /* readonly */
#define INA23X_BUS_VOLTAGE		0x02 /* readonly */
#define INA23X_POWER			0x03 /* readonly */
#define INA23X_CURRENT			0x04 /* readonly */
#define INA23X_CALIBRATION		0x05

/* INA231 register definitions */
#define INA231_MASK_ENABLE		0x06
#define INA231_ALERT_LIMIT		0x07

/* register count */
#define INA231_REGISTERS		8

/* settings - depend on use case */
#define INA231_CONFIG_DEFAULT		0x4527	/* Averages = 16, CT = 1.1 ms, triggered */
#define INA231_CALIB_DEFAULT        0x42AB  /* 30 mOhm Shunt and 0.01 mA LSB */

#define INA2XX_RSHUNT_DEFAULT		30000 /* In uOhms */

/* bit number of alert functions in Mask/Enable Register */
#define INA231_SHUNT_OVER_VOLTAGE_BIT	15
#define INA231_SHUNT_UNDER_VOLTAGE_BIT	14
#define INA231_BUS_OVER_VOLTAGE_BIT	    13
#define INA231_BUS_UNDER_VOLTAGE_BIT	12
#define INA231_POWER_OVER_LIMIT_BIT	    11
#define INA231_CONVERSION_READY_BIT	    10

/* bit mask for alert config bits of Mask/Enable Register */
#define INA231_ALERT_CONFIG_MASK	    0xFC00
#define INA231_CONVERSION_READY_FLAG	BIT(3)
#define INA231_ALERT_FUNCTION_FLAG	    BIT(4)

struct ina23x_data {
    const struct i2c_dt_spec devSpec;
	long rshunt;
	long current_lsb_uA;
	long power_lsb_uW;
};

/* PUBLIC FUNCTION PROTOTYPES *************************************************/
bool ina23x_available(const struct ina23x_data *spec);
int ina23x_init(struct ina23x_data *spec, int rShunt, int currentLSB);
int ina23x_read(struct ina23x_data *spec, uint8_t reg, uint16_t *buf);
int ina23x_write(struct ina23x_data *spec, uint8_t reg, uint16_t buf);
int ina23x_format_read(struct ina23x_data *spec, uint8_t reg, int *buf);
int ina23x_alert_enable_set(struct ina23x_data *spec, uint16_t bitmask, bool pol, bool latch);
int ina23x_alert_enable_read(struct ina23x_data *spec, uint16_t *buf);
bool ina23x_conversion_ready(struct ina23x_data *spec);
bool ina23x_power_down(struct ina23x_data *spec);
bool ina23x_power_up(struct ina23x_data *spec);

#endif /* INA231_H_ */