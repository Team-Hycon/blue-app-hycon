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

void get_compressed_public_key_value(unsigned char *value, unsigned char *out) {
	memcpy(out, value, 65);
	out[0] = ((out[64] & 1) ? 0x03 : 0x02);
}

void get_address_string_from_key(const cx_ecfp_public_key_t publicKey,
                                 uint8_t *out) {
	hycon_hash_t hashAddress;

	// Compressed public key
	unsigned char tmpPubKey[65];
	get_compressed_public_key_value(publicKey.W, tmpPubKey);

	blake2b(hashAddress, sizeof(hycon_hash_t), tmpPubKey, 32, &G_blake2b_state, 0);

	os_memmove(out, hashAddress + 12, 20);
}

uint32_t set_result_publicKey(cx_ecfp_public_key_t publicKey) {
	uint32_t tx = 0;
	G_io_apdu_buffer[tx++] = 32;

	// Compressed public key
	unsigned char tmpPubKey[65];
	get_compressed_public_key_value(publicKey.W, tmpPubKey);
	os_memmove(G_io_apdu_buffer + tx, tmpPubKey, 32);
	tx += 32;

	uint8_t address[21];
	get_address_string_from_key(publicKey, address);
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
