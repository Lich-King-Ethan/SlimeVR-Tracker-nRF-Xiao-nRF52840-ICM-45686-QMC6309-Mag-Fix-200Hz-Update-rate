#include "qmc6309.h"

static const struct device *qmc_i2c;
static uint8_t qmc_addr;

static bool qmc_read_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    return (i2c_burst_read(qmc_i2c, qmc_addr, reg, data, len) == 0);
}

static bool qmc_write_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = { reg, val };
    return (i2c_write(qmc_i2c, buf, 2, qmc_addr) == 0);
}

bool qmc6309_init(const struct device *i2c_dev, uint8_t address) {
    qmc_i2c = i2c_dev;
    qmc_addr = address;

    uint8_t chip_id = 0;
    if (!qmc_read_reg(QMC6309_CHIP_ID_REG, &chip_id, 1)) {
        LOG_ERR("QMC6309: Cannot read chip ID");
        return false;
    }

    if (chip_id != QMC6309_CHIP_ID_VAL) {
        LOG_ERR("QMC6309: Wrong chip ID: 0x%02X", chip_id);
        return false;
    }

    LOG_INF("QMC6309 detected (ID=0x%02X)", chip_id);

    // Reset
    qmc_write_reg(QMC6309_REG_CONTROL2, 0x80);
    k_msleep(10);

    // ODR = 200 Hz, OSR = 512, Range = ±2G
    // Control1 bits:
    // OSR(7:6)=11 → 512
    // RNG(5:4)=00 → 2G
    // ODR(3:2)=11 → 200Hz
    // MODE(1:0)=01 → Continuous
    uint8_t control1 = 0b11001101;
    qmc_write_reg(QMC6309_REG_CONTROL1, control1);

    LOG_INF("QMC6309 initialized @200Hz");
    return true;
}

bool qmc6309_read(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t status = 0;
    if (!qmc_read_reg(QMC6309_REG_STATUS, &status, 1))
        return false;

    if ((status & QMC6309_DRDY_BIT) == 0)
        return false; // No new data yet

    uint8_t raw[6];
    if (!qmc_read_reg(QMC6309_REG_XOUT_L, raw, 6))
        return false;

    *x = (int16_t)((raw[1] << 8) | raw[0]);
    *y = (int16_t)((raw[3] << 8) | raw[2]);
    *z = (int16_t)((raw[5] << 8) | raw[4]);

    return true;
}
