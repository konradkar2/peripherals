#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c.h>
#include <util/twi.h>

#define LOG() printf("%s\n", __func__)

void i2c_driver_init(i2c_driver *drv, uint8_t address)
{
	LOG();
	drv->address = address;
}

int i2c_driver_start(i2c_driver *drv)
{
	LOG();
	// Clear TW interrupt flag, Set TW start, set TW enable
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	// wait for completion
	LOG();
	while (!(TWCR & (1 << TWINT))) {
	}

	// load slave address, and wait for ACK
	TWDR = drv->address;
	// transmit slave address
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait for ACK/NACK
	LOG();
	while (!(TWCR & (1 << TWINT))) {
	}

	LOG();
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK) {
		return 1;
	}
}

void i2c_driver_stop(i2c_driver *drv)
{
	(void)drv;
	LOG();
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

int i2c_driver_transmit(i2c_driver *drv, uint8_t data)
{
	(void)drv;
	LOG();

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	LOG();
	// wait for ACK/NACK
	while (!(TWCR & (1 << TWINT))) {
	}

	LOG();
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		return 1;
	}
	return 0;
}
uint8_t spi_driver_read() { return 0; }