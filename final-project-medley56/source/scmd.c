/*
 * scmd.c
 *
 *  Created on: Dec 7, 2023
 *      Author: gmedley
 */

#include "scmd.h"

#include <assert.h>
#include "MKL25Z4.h"

/* I2C1 Configuration settings */
#define I2C I2C1
#define SDA_PIN (0U)  // PTE0
#define SCL_PIN (1U)  // PTE1
#define MUX_MODE (6U)  // For PTE0 and PTE1 this sets them to I2C1
#define BUS_CLOCK_FREQ (24000000U)  // 24MHz
#define MULT_VAL (0U)  // 38.3.2 I2C Frequency Divider register (I2Cx_F)
#define ICR_VAL (0x1FU)  // 0x1F => SCL div of 240. See 38.4.1.10 I2C divider and hold values

/* Shortcut operations
 * Adapted from Dean */
#define I2C_M_START I2C1->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP I2C1->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_REPSTART I2C1->C1 |= I2C_C1_RSTA_MASK
#define I2C_SET_TX I2C1->C1 |= I2C_C1_TX_MASK
#define I2C_SET_RX I2C1->C1 &= ~I2C_C1_TX_MASK
#define I2C_WAIT i2c_wait()
#define NACK I2C1->C1 |= I2C_C1_TXAK_MASK  // Do not send acks
#define ACK I2C1->C1 &= ~I2C_C1_TXAK_MASK  // Send acks

/* Shortcuts for setting read/write to device */
#define DEV_READ(dev) (dev << 1 | 0x01)  // Shift (7-bit) device address over and assert R/W bit
#define DEV_WRITE(dev) (dev << 1)  // Shift (7-bit) device address over and leave R/W bit 0


void scmd_init (void)
{
    /* Send clocks to I2C1 and PORTE */
    SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;  // 12.2.8 System Clock Gating Control Register 4 (SIM_SCGC4)
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;  // 12.2.9 System Clock Gating Control Register 5 (SIM_SCGC5)

    /* Set MUX mode to I2C1_SDA/SCL for pins PTE0 and PTE1
     * See 10.3.1 KL25 Signal Multiplexing and Pin Assignments
     * in reference manual */
    PORTE->PCR[SDA_PIN] = PORT_PCR_MUX(MUX_MODE);
    PORTE->PCR[SCL_PIN] = PORT_PCR_MUX(MUX_MODE);

    /* Set baud rate
     * I2C baud rate = bus speed (Hz)/(mul × SCL divider)
     * ICR = 0x1F sets SCL_DIV = 240
     * MULT = 0 sets multiplier = 1
     * => baud rate = 24MHz / (1 * 240) = 100kHz
     * */
    I2C1->F = I2C_F_ICR(ICR_VAL) | I2C_F_MULT(MULT_VAL);  // 38.3.2 I2C Frequency Divider register (I2Cx_F)

    /* Enable I2C */
    I2C1->C1 |= (I2C_C1_IICEN_MASK);  // | I2C_C1_IICIE_MASK);

    /* Set high drive mode */
    I2C1->C2 |= I2C_C2_HDRS_MASK;

    assert(scmd_read_byte(SCMD_DEV, SCMD_ID) == SCMD_ID_DEFAULT);
}


uint8_t scmd_fid(void)
{
    return scmd_read_byte(SCMD_DEV, SCMD_FID);
}


uint8_t scmd_id(void)
{
    return scmd_read_byte(SCMD_DEV, SCMD_ID);
}


void scmd_enable(void)
{
    scmd_write_byte(SCMD_DEV, SCMD_DRIVER_ENABLE, 1);
}


void scmd_disable(void)
{
    scmd_write_byte(SCMD_DEV, SCMD_DRIVER_ENABLE, 0);
}


void scmd_ma_set_invert(uint8_t invert)
{
    scmd_write_byte(SCMD_DEV, SCMD_MOTOR_A_INVERT, invert);
}


void scmd_ma_set_drive(uint8_t drive)
{
    scmd_write_byte(SCMD_DEV, SCMD_MA_DRIVE, drive);
}


void i2c_wait (void)
{
    while ((I2C1->S & I2C_S_IICIF_MASK) == 0)
    {
        // Wait for interrupt flag to be set
    }
    I2C1->S |= I2C_S_IICIF_MASK;  // Write to flag to clear it
}


void scmd_write_byte(uint8_t device, uint8_t offset, uint8_t data)
{
    I2C_SET_TX;
    I2C_M_START;
    I2C1->D = DEV_WRITE(device);
    I2C_WAIT;

    I2C1->D = offset;
    I2C_WAIT;

    I2C1->D = data;
    I2C_WAIT;
    I2C_M_STOP;
}


uint8_t scmd_read_byte(uint8_t device, uint8_t offset)
{
    uint8_t data;

    I2C_SET_TX;  // Set transmit
    I2C_M_START;  // Send start signal
    I2C1->D = DEV_WRITE(device);  // Send device address
    I2C_WAIT;  // Wait for ACK

    I2C1->D = offset;  // Send register from which to eventually read
    I2C_WAIT;  // Wait for ACK

    I2C_M_REPSTART;  // Send start signal again without losing master arbitration (ensures uninterrupted)
    I2C1->D = DEV_READ(device);  // Send device address with read request bit
    I2C_WAIT;  // Wait for ACK

    I2C_SET_RX;  // Set receive
    NACK;  // Do not send acks

    data = I2C1->D;  // Initiate a dummy read to trigger reading the incoming next byte
    I2C_WAIT;  // Wait for ACK

    I2C_M_STOP;  // Send stop signal
    data = I2C1->D;  // Read data register

    return data;
}
