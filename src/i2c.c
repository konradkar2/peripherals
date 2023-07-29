#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c.h>
#include <stdio.h>
#include <util/twi.h>

#define LOG() printf("%s\n", __func__)

#define WAIT_FOR_TWI()                                                         \
	while (!(TWCR & (1 << TWINT))) {                                       \
	}

#define TW_STATUS_ASSERT(EXPECTED_TW_STATUS)                                   \
	{                                                                      \
		const uint8_t tw_status = TW_STATUS;                           \
		if (EXPECTED_TW_STATUS != tw_status) {                         \
			printf("ERR: %s (%d): tw status NOK: actual 0x%02x, "  \
			       "expected: "                                    \
			       "0x%02x (%s) \n",                               \
			       __func__, __LINE__, tw_status,                  \
			       EXPECTED_TW_STATUS, #EXPECTED_TW_STATUS);       \
			return tw_status;                                      \
		}                                                              \
	}

void i2c_driver_init(i2c_driver *drv, uint8_t address)
{
	LOG();
	drv->address = address;
}

int8_t i2c_driver_start(i2c_driver *drv)
{
	LOG();
	// Clear TW interrupt flag, Set TW start, set TW enable
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	LOG();
	WAIT_FOR_TWI();
	TW_STATUS_ASSERT(TW_START);

	// load slave address, and wait for ACK
	// set last bit to 0 (write)
	TWDR = drv->address & 0xFE;
	// transmit slave address
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait for slave ACK/NACK
	LOG();
	WAIT_FOR_TWI();
	TW_STATUS_ASSERT(TW_MT_SLA_ACK);

	return 0;
}

int8_t i2c_driver_stop()
{
	LOG();
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	WAIT_FOR_TWI();
	TW_STATUS_ASSERT(TW_SR_STOP);

	return 0;
}

int8_t i2c_driver_transmit(uint8_t data)
{
	LOG();
	if (!(TWCR & (1 << TWINT))) {
		return 1;
	}

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	LOG();
	// wait for ACK/NACK
	WAIT_FOR_TWI();
	TW_STATUS_ASSERT(TW_MT_DATA_ACK)
	return 0;
}

int8_t i2c_driver_read(i2c_driver *drv, const size_t n, uint8_t *out)
{
	LOG();
	// Clear TW interrupt flag, Set TW start, set TW enable
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	LOG();
	// wait for completion
	WAIT_FOR_TWI();
	TW_STATUS_ASSERT(TW_START)

	// load slave address, and wait for ACK
	// set last bit to 1 (read)
	TWDR = drv->address | 0x01;

	// transfer
	TWCR = (1 << TWINT) | (1 << TWEN);

	LOG();
	// wait for response
	while (!(TWCR & (1 << TWINT))) {
	}

	// check for master read ACK
	TW_STATUS_ASSERT(TW_MR_SLA_ACK)

	// read n-1 bytes
	for (size_t i = 0; i < n - 1; ++i) {
		// receive data and wait for ACK
		TWCR = (1 << TWINT) | (1 << TWEA);
		// wait for data
		WAIT_FOR_TWI();
		TW_STATUS_ASSERT(TW_MR_DATA_ACK)
		*out = TWDR;
		++out;
	}

	// read last byte and transmit NACK
	TWCR = (1 << TWINT);
	*out = TWDR;

	TW_STATUS_ASSERT(TW_MR_DATA_NACK)
	return 0;
}