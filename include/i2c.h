#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <pin.h>

struct i2c_driver
{
    uint8_t address;
};
typedef struct i2c_driver i2c_driver;

void i2c_driver_init(i2c_driver *drv, uint8_t address);
int i2c_driver_start(i2c_driver *drv);
void i2c_driver_stop(i2c_driver *drv);
int i2c_driver_transmit(i2c_driver * drv, uint8_t cData);
uint8_t spi_driver_read();

#endif