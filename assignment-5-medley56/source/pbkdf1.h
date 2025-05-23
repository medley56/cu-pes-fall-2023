/**
 * @file pbkdf.h
 * @brief PBKDF (Password-Based Key Derivation Function)
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2023 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCRYPTO Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 2.2.4
 **/

#ifndef _PBKDF_H
#define _PBKDF_H

//Dependencies
#include <stddef.h>
#include <stdint.h>

#include "error.h"

//C++ guard
#ifdef __cplusplus
 extern "C" {
 #endif

//PBKDF related constants
extern const uint8_t PBKDF2_OID[9];

//PBKDF related functions
error_t pbkdf1(const uint8_t *p, size_t pLen, const uint8_t *s, size_t sLen,
		uint32_t c, uint8_t *dk, size_t dkLen);

//C++ guard
#ifdef __cplusplus
 }
 #endif

#endif
