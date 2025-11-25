#pragma once
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(QMC6309, LOG_LEVEL_INF);

// 7-bit I2C address for write=0x7C / read=0x7D
#define QMC6309_I2C_ADDR     0x3E

// Registers
#define QMC6309_REG_XOUT_L   0x01
#define QMC6309_REG_STATUS   0x06
#define QMC6309_REG_CONTROL1 0x21
#define QMC6309_REG_CONTROL2 0x22

// Status bits
#define QMC6309_DRDY_BIT     0x01

// ID check
#define QMC6309_CHIP_ID_REG  0x00
#define QMC6309_CHIP_ID_VAL  0x90

struct qmc6309_config {
    const struct device *i2c_dev;
    uint16_t i2c_addr;
};

bool qmc6309_init(const struct device *i2c_dev, uint8_t address);
bool qmc6309_read(int16_t *x, int16_t *y, int16_t *z);
