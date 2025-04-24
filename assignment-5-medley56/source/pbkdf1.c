
 /**
  * @file pbkdf.c
  * @brief PBKDF (Password-Based Key Derivation Function)
  *
  * @section License
  *
  * The following file has been modified from the original implementation available at
  * the Oryx Embedded SRL. This is a true implementation of pbkdf1 but with a fake
  * hash function isha, in this case.
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


 //Dependencies
#include <string.h>
#include "pbkdf1.h"
#include "isha.h"


 /**
  * @brief PBKDF1 key derivation function
  *
  * PBKDF1 applies a hash function to derive
  * keys. The length of the derived key is bounded by the length of the hash
  * function output, which is 16 octets for MD2 and MD5 and 20 octets for SHA-1
  *
  * @param[in] p Password, an octet string
  * @param[in] pLen Length in octets of password
  * @param[in] s Salt, an octet string
  * @param[in] sLen Length in octets of salt
  * @param[in] c Iteration count
  * @param[out] dk Derived key
  * @param[in] dkLen Intended length in octets of the derived key
  * @return Error code
  **/

 error_t pbkdf1(const uint8_t *p, size_t pLen,
    const uint8_t *s, size_t sLen, uint32_t c, uint8_t *dk, size_t dkLen)
 {
    uint32_t i;
    uint8_t t[ISHA_DIGESTLEN];
    ISHAContext *hashContext=(ISHAContext *)malloc(sizeof(ISHAContext));

    //Check parameters
    if(p == NULL || s == NULL || dk == NULL)
       return ERROR_INVALID_PARAMETER;

    //The iteration count must be a positive integer
    if(c < 1)
       return ERROR_INVALID_PARAMETER;

    //Check the intended length of the derived key
    if(dkLen > ISHA_DIGESTLEN)
       return ERROR_INVALID_LENGTH;

    //Apply the hash function to the concatenation of P and S
    ISHAReset(hashContext);
    ISHAInput(hashContext, p, pLen);
    ISHAInput(hashContext, s, sLen);
    ISHAResult(hashContext, t);

    //Iterate as many times as required
    for(i = 1; i < c; i++)
    {
       //Apply the hash function to T(i - 1)
       ISHAReset(hashContext);
       ISHAInput(hashContext, t, ISHA_DIGESTLEN);
       ISHAResult(hashContext, t);
    }

    //Output the derived key DK
    memcpy(dk, t, dkLen);

    //Free previously allocated memory
    free(hashContext);

    //Successful processing
    return NO_ERROR;
 }
