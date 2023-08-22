#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c.h>
#include <stdio.h>
#include <util/twi.h>

#define LOG() printf("%s\n", __func__)

#define TW_STATUS_ASSERT(EXPECTED_TW_STATUS)                                   \
	do {                                                                   \
		const uint8_t tw_status = TW_STATUS;                           \
		if (EXPECTED_TW_STATUS != tw_status) {                         \
			printf("ERR: %s:%d tw status NOK: actual 0x%02x, "     \
			       "expected: "                                    \
			       "0x%02x (%s) \n",                               \
			       __func__, __LINE__, tw_status,                  \
			       EXPECTED_TW_STATUS, #EXPECTED_TW_STATUS);       \
			return tw_status;                                      \
		}                                                              \
	} while (0)

static inline void wait_for_tw()
{
	while (!(TWCR & (1 << TWINT))) {
	}
}

void i2c_init(i2c_driver *drv, uint8_t address)
{
	drv->START_transmitted = 0;
	drv->address	       = address;

	// Enable pull ups
	PORTC |= (1 << PORT5 | 1 << PORT4);

	// Set the I2C prescaler
	TWSR = 0; // Set prescaler to 1 (optional)

	// Set the I2C bit rate (for 100 kHz operation)
	TWBR = ((F_CPU / 100000) - 16) / 2;
}

int8_t i2c_start(i2c_driver *drv)
{
	// Clear TW interrupt flag, Set TW start, set TW enable
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	wait_for_tw();
	if (drv->START_transmitted)
		TW_STATUS_ASSERT(TW_REP_START);
	else
		TW_STATUS_ASSERT(TW_START);

	drv->START_transmitted = 1;

	return 0;
}

int8_t i2c_send_address(i2c_driver *drv, i2c_driver_op operation)
{
	printf("%s addr: 0x%02x (%s)\n", __func__, drv->address,
	       operation == i2c_driver_op_read ? "read" : "write");

	// load slave address, and wait for ACK
	// set last bit to 0 (write), 1 (read)

	TWDR = (drv->address << 1);
	if (operation == i2c_driver_op_read)
		TWDR |= 0x01;

	// transmit slave address
	TWCR = (1 << TWINT) | (1 << TWEN);
	wait_for_tw();

	if (operation == i2c_driver_op_write)
		TW_STATUS_ASSERT(TW_MT_SLA_ACK);
	else
		TW_STATUS_ASSERT(TW_MR_SLA_ACK);

	return 0;
}

int8_t i2c_stop(i2c_driver *drv)
{
	drv->START_transmitted = 0;

	LOG();
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWSTO))) {
	}

	return 0;
}

int8_t i2c_transmit(uint8_t data)
{
	printf("%s data: 0x%02x\n", __func__, data);
	if (!(TWCR & (1 << TWINT))) {
		return 1;
	}

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	// wait for ACK/NACK
	wait_for_tw();
	TW_STATUS_ASSERT(TW_MT_DATA_ACK);
	return 0;
}

int8_t i2c_read(i2c_driver *drv, const size_t n, uint8_t *out)
{
	LOG();
	if (i2c_start(drv)) {
		return 1;
	}

	if (i2c_send_address(drv, i2c_driver_op_read)) {
		return 1;
	}

	// read n-1 bytes
	for (size_t i = 0; i < n - 1; ++i) {
		// receive data and send ACK
		TWCR = (1 << TWINT) | (1 << TWEA);
		// wait for data
		wait_for_tw();
		TW_STATUS_ASSERT(TW_MR_DATA_ACK);
		*out = TWDR;
		++out;
	}

	// read last byte and transmit NACK
	TWCR = (1 << TWINT) | (1 << TWEA);
	wait_for_tw();

	*out = TWDR;

	TW_STATUS_ASSERT(TW_MR_DATA_NACK);
	return 0;
}