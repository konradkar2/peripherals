#ifndef I2C_H
#define I2C_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <pin.h>
#include <stddef.h>
#include <stdint.h>

struct i2c_driver {
	uint8_t address;
};
typedef struct i2c_driver i2c_driver;

void i2c_driver_init(i2c_driver *drv, uint8_t address);
int8_t i2c_driver_start(i2c_driver *drv);
int8_t i2c_driver_stop();
int8_t i2c_driver_transmit(uint8_t cData);
int8_t i2c_driver_read(i2c_driver *drv, size_t n, uint8_t *out);

#endif