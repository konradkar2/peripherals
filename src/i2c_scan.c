#include <i2c.h>
#include <i2c_scan.h>
#include <stdio.h>

static int8_t i2c_probe(const uint8_t address)
{
	int8_t ret = 0;
	i2c_driver driver;

	i2c_driver_init(&driver, address);
	if (i2c_driver_start(&driver, i2c_driver_op_read)) {
		ret = 1;
	}
	i2c_driver_stop(&driver);

	return ret;
}

void i2c_scan()
{
    printf("%s start...\n", __func__);
    
	int8_t found_any = 0;
	for (uint8_t addr = 1; addr <= 128; ++addr) {
		if (!i2c_probe(addr)) {
			printf("found i2c device at: 0x%02x\n", addr);
			found_any = 1;
		}
	}

	if (found_any == 0) {
		printf("failed to find any i2c device\n");
	}
}