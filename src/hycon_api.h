/******************************************************************************
*   $HYCON Wallet for Ledger Nano S
*   (c) 2018 Dulguun Batmunkh
*   (c) 2018 Hycon
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
******************************************************************************/

#ifndef HYCON_API_H
#define HYCON_API_H

#include "hycon_types.h"

/* ------------------------------------------------------------------------- */
/* ---                            APDU functions                         --- */
/* ------------------------------------------------------------------------- */

void handleApdu(volatile unsigned int *flags, volatile unsigned int *tx);
void handleSign(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                uint16_t data_length, volatile unsigned int *flags,
                volatile unsigned int *tx);
void handleGetPublicKey(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                        uint16_t data_length, volatile unsigned int *flags,
                        volatile unsigned int *tx);
void handleGetAppConfig(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                        uint16_t data_length, volatile unsigned int *flags,
                        volatile unsigned int *tx);

/* ------------------------------------------------------------------------- */
/* ---                            Cryptography                           --- */
/* ------------------------------------------------------------------------- */

void get_compressed_public_key_value(unsigned char *value, unsigned char *out);
void get_address_string_from_key(const cx_ecfp_public_key_t publicKey,
                                 uint8_t *out);
uint32_t set_result_publicKey(cx_ecfp_public_key_t publicKey);
void int_to_displayable_chars(uint64_t number, char *out);

/* ------------------------------------------------------------------------- */
/* ---                            Others                                 --- */
/* ------------------------------------------------------------------------- */

bool decode_tx(uint8_t *data, size_t data_len, hycon_tx *tx_content);
void bin_addr_to_displayable_chars(uint8_t addr[21], char* out);

#endif 	// HYCON_API_H
