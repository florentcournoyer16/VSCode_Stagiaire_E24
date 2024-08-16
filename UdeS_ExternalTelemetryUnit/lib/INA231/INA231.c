/** @file       INA231.h
 *  @brief      INA231 control function.
 * 
 *  @copyright  Copyright (c) 2016 Intel Corporation.
 *
 *  @author     Jérémy Ménard.
 *  @date       Created on december 13th, 2023, 10:40 PM
 */

/* INCLUDES *******************************************************************/
#include "INA231.h"

/* PUBLIC FUNCTIONS ***********************************************************/

/** @brief Check if ina23x exist on the I2C BUS.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 *
 * @return TRUE if available and FALSE if not
 */
bool ina23x_available(const struct ina23x_data *spec){
    if(!device_is_ready(spec->devSpec.bus)){
        printk("ina@%x is not avaialble!\n\r",spec->devSpec.addr);
		return 0;
    }
    return 1;
}

/** @brief Initialize the ina23x with config and calibration values.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param rShunt Shunt resistance value in uOhms.
 * @param currentLSB Current LSB value in uA.
 *
 * @return TRUE if successful and FALSE if not
 */
int ina23x_init(struct ina23x_data *spec, int rShunt, int currentLSB){
    int err = 0;
    uint16_t temp;
    
    // Waiting for i2c readiness before writing the calibration
	while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_write(spec,INA23X_CONFIG,INA231_CONFIG_DEFAULT);
    if(err){
        printk("Failed to write the configuration to the ina\n");
        return 1;
    }

    temp = ceil((float)5120000000/(currentLSB*rShunt));

    // Waiting for i2c readiness before writing the calibration
	while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_write(spec,INA23X_CALIBRATION,temp);
    if(err){
        printk("Failed to write the calibration to the ina\n");
        return 2;
    }

    spec->current_lsb_uA = currentLSB; /* uA */
    spec->rshunt = rShunt; /* uOhms */
    spec->power_lsb_uW = spec->current_lsb_uA*25; /* 25 times current LSB */

    return 0;
}

/** @brief Read data in a specific register from the ina23x.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param reg ina23x register. Choose between INA23X_CONFIG,
 * INA23X_SHUNT_VOLTAGE, INA23X_BUS_VOLTAGE, INA23X_POWER,
 * INA23X_CURRENT, INA23X_CALIBRATION, INA23X_MASK_ENABLE and
 * INA231_ALERT_LIMIT, Mask/Enable and Alert Limit.
 * @param buf Memory pool that stores the retrieved data.
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_read(struct ina23x_data *spec, uint8_t reg, uint16_t *buf){
    uint8_t data[2] = {(*buf & 0xFF00 >> 8),(*buf & 0x00FF)};
    uint32_t numByte = 2;
    int err = 0;

    if (reg < INA23X_CONFIG || reg > INA231_ALERT_LIMIT){
        printk("Invalid ina23x register selected! (0x%x does not exist)\n",reg);
        return 1;
    }

    // Reading the I2C register using burst read since 2 bytes long.
    while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = i2c_burst_read_dt(&spec->devSpec,reg,&data[0],numByte);
    *buf = ((uint16_t)data[0] << 8) | data[1];
	if(err){
		printk("ina burst read error (err %i)\n", err);
        return err;
    }
    return 0;
}

/** @brief Write data in a specific register from the ina23x.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param reg ina23x register. Choose between INA23X_CONFIG,
 * INA23X_CALIBRATION, INA23X_MASK_ENABLE and INA231_ALERT_LIMIT.
 * @param buf Memory pool from which the data is transferred.
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_write(struct ina23x_data *spec, uint8_t reg, uint16_t buf){
    uint8_t data[2] = {(buf >> 8),(buf & 0xFF)};
    uint32_t numByte = 2;
    int err = 0;

    if (reg < INA23X_CONFIG || reg > INA231_ALERT_LIMIT){
        printk("Invalid ina23x register selected! (0x%x does not exist)\n",reg);
        return 1;
    }else if (reg >= INA23X_SHUNT_VOLTAGE && reg <= INA23X_CURRENT){
        printk("Read-only ina23x register selected! (0x%x)\n",reg);
        return 2;
    }

    // Waiting for i2c readiness before writing the config
    while(!i2c_is_ready_dt(&spec->devSpec)){}
	err = i2c_burst_write_dt(&spec->devSpec,reg,&data[0],numByte);
    if(err){
        printk("Failed to write ina register 0x%x\n", reg);
        return err;
    }
    return 0;
}

/** @brief Read data in a specific register from the ina23x and format it.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param reg ina23x register. Choose between INA23X_CONFIG,
 * INA23X_SHUNT_VOLTAGE, INA23X_BUS_VOLTAGE, INA23X_POWER,
 * INA23X_CURRENT, INA23X_CALIBRATION, INA23X_MASK_ENABLE and
 * INA231_ALERT_LIMIT, Mask/Enable and Alert Limit.
 * @param buf Memory pool that stores the retrieved data. Shunt = uV,
 * Bus = mV, Power = uW and Current = uA.
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_format_read(struct ina23x_data *spec, uint8_t reg, int *buf){
    int err = 0;
    uint16_t tempRead = 0;

    while(!ina23x_conversion_ready(spec)){}
    err = ina23x_read(spec,reg,&tempRead);
    if(err){
        printk("ina23x read error (err %d)\n", err);
        return err;
    }

    switch (reg){
    case INA23X_SHUNT_VOLTAGE:
        // LSB is 2.5 uV (output in uV)
        *buf = round(tempRead*2.5);
        break;
    case INA23X_BUS_VOLTAGE:
        // LSB is 1.25 mV (output in mV)
        *buf = round(tempRead*1.25);
        break;
    case INA23X_POWER:
        // LSB is 25x current LSB (output in uW)
        *buf = round(tempRead*spec->power_lsb_uW);
        break;
    case INA23X_CURRENT:
        // LSB is given in initialization (output in uA)
        *buf = round(tempRead*spec->current_lsb_uA);
        break;
    case INA23X_CONFIG:
    case INA23X_CALIBRATION:
    case INA231_MASK_ENABLE:
    case INA231_ALERT_LIMIT:
        *buf = tempRead;
        break;
    default:
        printk("Invalid ina23x register selected! (0x%x does not exist)\n",reg);
        return 1;
        break;
    }

    return 0;

}

/** @brief Write to the MASK/ENABLE register to set the alert.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param bitmask Function that triggers the alert pin. Choose between
 * INA231_SHUNT_OVER_VOLTAGE_BIT, INA231_SHUNT_UNDER_VOLTAGE_BIT,
 * INA231_BUS_OVER_VOLTAGE_BIT, INA231_BUS_UNDER_VOLTAGE_BIT,
 * INA231_POWER_OVER_LIMIT_BIT and INA231_CONVERSION_READY_BIT.
 * @param pol Polarity for the alert pin. TRUE = Active-high and False = Active-low.
 * @param latch Enable latch alert (need to read Mask/Enable register to reset alert).
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_alert_enable_set(struct ina23x_data *spec, uint16_t bitmask, bool pol, bool latch){
    int err = 0;
    uint16_t temp = BIT(bitmask) + (pol<<1) + latch;

    err = ina23x_write(spec, INA231_MASK_ENABLE, temp);
    if(err){
        return 1;
    }
    
    return 0;

}

/** @brief Read data of the MASK/ENABLE register.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param buf Memory pool from which the data is received.
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_alert_enable_read(struct ina23x_data *spec, uint16_t *buf){
    int err = 0;

    err = ina23x_read(spec, INA231_MASK_ENABLE, buf);
    if(err){
        return err;
    }
    
    return 0;

}

/** @brief Set the alert limit for the ina23x.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 * @param buf Memory pool from which the data is transferred.
 *
 * @retval 0 If successful.
 * @retval -output error number.
 */
int ina23x_alert_limit_set(struct ina23x_data *spec, uint16_t buf){
    int err = 0;

    err = ina23x_write(spec, INA231_ALERT_LIMIT, buf);
    if(err){
        return err;
    }
    
    return 0;

}

/** @brief See if conversion is ready.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 *
 * @retval TRUE is ready. FALSE if not ready.
 */
bool ina23x_conversion_ready(struct ina23x_data *spec){
    int err = 0;
    uint16_t temp = 0;
    uint16_t isReady = 0;

    while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_read(spec, INA231_MASK_ENABLE, &temp);
    if(err){
        return 0;
    }

    isReady = (temp & INA231_CONVERSION_READY_FLAG)>>3;
    
    return isReady;

}

/** @brief Shutdown the ina23x to save power.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 *
 * @retval TRUE if successful. FALSE if not successful.
 */
bool ina23x_power_down(struct ina23x_data *spec){
    int err = 0;

    while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_write(spec, INA23X_CONFIG, 0x00);
    if(err){
        return 0;
    }

    return 1;
}

/** @brief Power up a ina23x.
 * 
 * @param spec ina23x object with DT spec and calibration values.
 *
 * @retval TRUE if successful. FALSE if not successful.
 */
bool ina23x_power_up(struct ina23x_data *spec){
    int err = 0;
    uint16_t temp = 0;

    while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_write(spec, INA23X_CONFIG, INA231_CONFIG_DEFAULT);
    if(err){
        return 0;
    }

    while(!i2c_is_ready_dt(&spec->devSpec)){}
    err = ina23x_read(spec, INA23X_CONFIG, &temp);
    if(err){
        return 0;
    }else if (temp != INA231_CONFIG_DEFAULT){
        printk("The ina could not power up !\n");
        return 0;
    }

    return 1;
}

