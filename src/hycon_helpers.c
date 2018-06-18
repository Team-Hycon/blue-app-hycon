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

#include "hycon_api.h"

#include "ram_variables.h"

uint64_t _decode_varint(uint8_t *buf, uint8_t *skip_bytes);

void get_compressed_public_key_value(unsigned char *value, unsigned char *out) {
	memcpy(out, value, 65);
	out[0] = ((out[64] & 1) ? 0x03 : 0x02);
}

void get_address_string_from_key(const cx_ecfp_public_key_t public_key,
                                 uint8_t *out) {
	hycon_hash_t hash_address;

	// Compressed public key
	unsigned char tmp_pub_key[65];
	get_compressed_public_key_value(public_key.W, tmp_pub_key);

	blake2b(hash_address, sizeof(hycon_hash_t), tmp_pub_key, 32, &G_blake2b_state, 0);

	os_memmove(out, hash_address + 12, 20);
}

uint32_t set_result_publicKey(cx_ecfp_public_key_t public_key) {
	uint32_t tx = 0;
	G_io_apdu_buffer[tx++] = 32;

	// Compressed public key
	unsigned char tmp_pub_key[65];
	get_compressed_public_key_value(public_key.W, tmp_pub_key);
	os_memmove(G_io_apdu_buffer + tx, tmp_pub_key, 32);
	tx += 32;

	uint8_t address[21];
	get_address_string_from_key(public_key, address);
	G_io_apdu_buffer[tx++] = 20;
	os_memmove(G_io_apdu_buffer + tx, address, 20);
	tx += 20;

	return tx;
}

void int_to_displayable_chars(uint64_t number, char *out) {
	char tmp[21];
	size_t i = 0;
	do {
		tmp[i] = number%10;
		i++;
	} while (number /= 10);

	// reverse array
	size_t j = 0;
	while (i) {
		i--;
		out[j] = tmp[i] + '0';
		j++;
	}

	out[j] = '\0';
}

// TODO: verify field number and type
bool decode_tx(uint8_t *buf, hycon_tx *tx_content) {
	size_t i;
	size_t len;

	// TO
	buf++;	// intentional skip
	len = *(buf++);
	for (i = 0; i < len; i++) {
		tx_content->to[i] = *(buf++);
	}
	tx_content->to[i] = '\0';

	// AMOUNT
	uint8_t skip_bytes;
	tx_content->amount = _decode_varint(buf, &skip_bytes);

	// FEE
	buf+=skip_bytes;
	tx_content->fee = _decode_varint(buf, &skip_bytes);

	return true;
}

uint64_t _decode_varint(uint8_t *buf, uint8_t *skip_bytes) {
	uint64_t result = 0;
	(*skip_bytes) = 0;
	uint64_t val;
	do {
		buf++;
		val = (*buf) & 0x7f;
		result |= (val << ((*skip_bytes)*7));
		(*skip_bytes)++;
	} while ((*buf) & 0x80);

	(*skip_bytes)++;
	return result;
}

void bin_addr_to_displayable_chars(uint8_t addr[21], char* out) {
	size_t i;
	for (i = 0; i < 20; i++) {
		out[i*2] = HEX_DIGITS[(addr[i]>>4) & 0x0f];
		out[i*2+1] = HEX_DIGITS[addr[i] & 0x0f];
	}
	out[i*2] = '\0';
}
