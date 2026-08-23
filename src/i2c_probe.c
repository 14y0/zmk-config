/*
 * ThreeKey debug: read TWIM0 PSEL registers 10s after boot to verify
 * whether I2C pins P0.29/P0.31 are actually configured at runtime.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx.h>

LOG_MODULE_REGISTER(i2c_probe, LOG_LEVEL_INF);

static void i2c_probe_work_handler(struct k_work *work)
{
	uint32_t psda = NRF_TWIM0->PSEL.SDA;
	uint32_t pscl = NRF_TWIM0->PSEL.SCL;

	LOG_INF("TWIM0 PSEL.SDA=0x%08X (pin %u port %u connect %u)", psda,
		psda & 0x1Fu, (psda >> 8) & 0x1u, (psda >> 31) & 1u);
	LOG_INF("TWIM0 PSEL.SCL=0x%08X (pin %u port %u connect %u)", pscl,
		pscl & 0x1Fu, (pscl >> 8) & 0x1u, (pscl >> 31) & 1u);
}

K_WORK_DELAYABLE_DEFINE(i2c_probe_work, i2c_probe_work_handler);

static int i2c_probe_init(void)
{
	k_work_schedule(&i2c_probe_work, K_SECONDS(10));
	return 0;
}

SYS_INIT(i2c_probe_init, APPLICATION, 90);
