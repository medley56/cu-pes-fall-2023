/*
 * scmd.h
 *
 * Module for controlling the SparkFun Serial Controlled Motor Driver (SCMD) over I2C.
 *
 *  Created on: Dec 7, 2023
 *      Author: gmedley
 */

#ifndef SCMD_H_
#define SCMD_H_

#include <stdint.h>

/* SCMD device address (unshifted) */
#define SCMD_DEV (0x5DU)  // Device address of motor controller (defined by hardware)
#define SCMD_ID_DEFAULT (0xA9)  // Hardware defined, unmodifiable register value to test against

/* Useful SCMD Registers */
#define SCMD_MA_DRIVE (0x20U)  // Register address of Motor A drive value (0-256)
#define SCMD_MOTOR_A_INVERT (0x12U)  // Register address of Motor A invert state
#define SCMD_DRIVER_ENABLE (0x70U)  // Enable all drivers (1 is enabled)
#define SCMD_ID (0x01U)  // ID Register
#define SCMD_FID (0x00U)  // Firmware ID register

/* Motor drive values for external controller use */
#define SCMD_MAX_FWD (0xFFU)
#define SCMD_ZERO_DRIVE (0x80U)
#define SCMD_MAX_REV (0x00U)


/*
 * @brief Initialize the SCMD via I2C
 */
void scmd_init(void);


/*
 * @brief Get the SCMD firmware ID
 *
 * @return The firmware ID of the SCMD device
 */
uint8_t scmd_fid(void);


/*
 * @brief Get the ID of the SCMD device
 *
 * @return The ID of the device (0xA9)
 */
uint8_t scmd_id(void);


/*
 * @brief Enable the SCMD
 */
void scmd_enable(void);


/*
 * @brief Disable the SCMD
 */
void scmd_disable(void);


/*
 * @brief Set inverted status of Motor A on SCMD
 *
 * By default this is set to 0 on the SCMD
 *
 * @param invert If 0, not inverted, if 1, inverted
 */
void scmd_ma_set_invert(uint8_t invert);


/*
 * @brief Set drive speed of motor A
 *
 * @param drive The drive speed (0-255)
 */
void scmd_ma_set_drive(uint8_t drive);


/*
 * @brief Write a byte of data to the SCMD
 *
 * Prefer using the abstract functions provided in this module.
 *
 * This code is partially adapted from the SparkFun Arduino library for serial motor drivers
 * https://github.com/sparkfun/SparkFun_Serial_Controlled_Motor_Driver_Arduino_Library/tree/master/src
 *
 * @param device Device address to write to (configured on the peripheral itself via jumpers)
 * @param offset Register address on peripheral device to write to (7-bit register)
 * @param data Byte to write to peripheral
 */
void scmd_write_byte(uint8_t device, uint8_t offset, uint8_t data);


/*
 * @brief Read a byte of data from a register on the SCMD
 *
 * Prefer using the abstract functions provided in this module.
 *
 * @param device Address of the device from which to read (configured on the SCMD itself via jumpers)
 * @param offset Register on the SCMD from which to read
 *
 * @return Byte read from the offset register on device
 */
uint8_t scmd_read_byte(uint8_t device, uint8_t offset);



#endif /* SCMD_H_ */
