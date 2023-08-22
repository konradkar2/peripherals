#ifndef I2C_H
#define I2C_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <pin.h>
#include <stddef.h>
#include <stdint.h>

struct i2c_driver {
	uint8_t address;
	int8_t START_transmitted;
};
typedef struct i2c_driver i2c_driver;
void i2c_init(i2c_driver *drv, uint8_t address);

enum i2c_driver_op { i2c_driver_op_read, i2c_driver_op_write };
typedef enum i2c_driver_op i2c_driver_op;

int8_t i2c_start(i2c_driver *drv);
int8_t i2c_send_address(i2c_driver *drv, i2c_driver_op operation);
int8_t i2c_stop(i2c_driver *drv);
int8_t i2c_transmit(uint8_t cData);
int8_t i2c_read(i2c_driver *drv, size_t n, uint8_t *out);

#endif